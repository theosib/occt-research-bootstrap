# 3D Offset Algorithm (BRepOffset)

OCCT version: 7.9.3
Packages: `BRepOffset`, `BRepOffsetAPI`

## Overview

OCCT's 3D offset creates a new shell/solid at a fixed distance from the input shape. The algorithm works **face-by-face**: each face is offset independently, then adjacent offset faces are intersected to form new edges.

This approach is fundamentally limited: it cannot "eat" geometry that becomes invalid when the offset distance exceeds local curvature.

## Entry Points

```cpp
// User-facing API
BRepOffsetAPI_MakeOffsetShape offsetter;
offsetter.PerformByJoin(shape, offset_distance, tolerance);
TopoDS_Shape result = offsetter.Shape();

// Also available: PerformBySimple() for fast path without intersections
```

The API delegates to `BRepOffset_MakeOffset`, which is the main algorithm orchestrator (~350KB of implementation).

## Algorithm Phases

### Phase 1: Analysis

```
BRepOffset_Analyse::Perform()
```

Classifies each edge by the relationship between its adjacent faces:

| Classification | Meaning | Offset behavior |
|---------------|---------|-----------------|
| `ChFiDS_Convex` | Faces open outward | Gap between offset faces (needs fill) |
| `ChFiDS_Concave` | Faces close inward | Offset faces may intersect/overlap |
| `ChFiDS_Tangential` | Faces are tangent | Shared edge, no gap |

This analysis determines how offset faces will be connected.

### Phase 2: Offset Face Generation

```
MakeOffsetFaces(MapSF, ...)
  → For each face F:
      BRepOffset_Offset OF(F, offset_distance, ...)
      MapSF.Bind(F, OF)
```

Each face is offset independently using `BRepOffset_Offset`:
- Planar faces → parallel plane at distance d
- Cylindrical faces → larger/smaller cylinder
- General surfaces → offset surface (complex)

**Key data structure:** `BRepOffset_DataMapOfShapeOffset MapSF` maps original face → offset object.

The offset object tracks status:
```cpp
enum BRepOffset_Status {
  BRepOffset_Good,        // Normal offset
  BRepOffset_Reversed,    // Offset > surface radius (inside-out)
  BRepOffset_Degenerated, // Offset = surface radius exactly
  BRepOffset_Unknown      // Non-analytic surface
};
```

### Phase 3: Face Extension

Before intersection, offset faces are extended beyond their original boundaries:

```
MES: Offset shape → Extended shape
```

Extension ensures faces overlap enough to compute intersections reliably.

### Phase 4: 3D Intersection

```
Inter3d.ConnexIntByInt()   // Adjacent offset faces
Inter3d.ContextIntByInt()  // Caps and boundary faces
```

Extended offset faces are intersected pairwise. Results stored in ancestor-descendant structure:

```cpp
Handle(BRepAlgo_AsDes) AsDes;  // face → edges, edge → vertices
```

### Phase 5: Edge Trimming

```
TrimEdges()
```

Edges are trimmed at intersection points. This is where `BRepOffset_CannotTrimEdges` errors occur.

### Phase 6: 2D Intersection

```
BRepOffset_Inter2d::Compute()
```

On each offset face, find edge-edge intersections in the face's parameter space. Creates new vertices where edges cross.

### Phase 7: Loop Reconstruction

```
myMakeLoops.Build(...)
```

From the trimmed, intersected edges, reconstruct closed loops (wires) that form valid face boundaries.

### Phase 8: Shell/Solid Assembly

```
MakeShells()
SelectShells()
MakeSolid()
```

Assemble faces into shells, select appropriate shells for result, form solid if input was solid.

## The "Offset > Radius" Failure

Consider offsetting a box with a rounded edge (fillet radius R) by distance d > R:

```
Before offset:          After attempting offset d > R:
    ___________
   /           \          The curved face "reverses" - its
  |   curved    |         offset surface points inward instead
  |   face R    |         of outward. The planar offset faces
  |_____________|         now have nothing valid to intersect with.
```

The algorithm detects this via `BRepOffset_Reversed` status, but has no mechanism to:
1. Remove the invalid curved face
2. Extend the adjacent planar faces to meet each other
3. Reconstruct a sharp corner

This would require fundamentally different logic: "geometry eating" rather than "geometry offsetting."

## Error Codes

```cpp
enum BRepOffset_Error {
  BRepOffset_NoError,
  BRepOffset_UnknownError,
  BRepOffset_BadNormalsOnGeometry,    // Surface normal issues
  BRepOffset_C0Geometry,              // Non-C1 surface continuity
  BRepOffset_NullOffset,              // Zero offset invalid
  BRepOffset_NotConnectedShell,       // Disconnected input
  BRepOffset_CannotTrimEdges,         // Edge trimming failed
  BRepOffset_CannotFuseVertices,      // Vertex merging failed
  BRepOffset_CannotExtentEdge,        // Edge extension failed
  BRepOffset_UserBreak,               // Cancelled
  BRepOffset_MixedConnectivity        // Partially C0 at edge
};
```

## Two Execution Modes

### Join Mode (default): `PerformByJoin()`

Full algorithm with intersection. Two sub-modes:

| Mode | Method | Use case |
|------|--------|----------|
| `GeomAbs_Arc` | Connect with pipe surfaces + spherical caps | Small offsets, simpler |
| `GeomAbs_Intersection` | Extend and intersect faces | Complex geometry |

### Simple Mode: `PerformBySimple()`

Fast path that just offsets surfaces without intersection computation. Creates larger tolerances to cover gaps. From the header:

> "The possible drawback of the simple algorithm is that it leads, in general case, to tolerance increasing."

## Tolerance Handling

```cpp
// Angle tolerance derived from linear tolerance
Standard_Real TolAngleCoeff = Min(myTol / (Abs(myOffset * 0.5) + Precision::Confusion()), 1.0);
Standard_Real TolAngle = 4 * ASin(TolAngleCoeff);
```

Key insight: tolerance is *relative* to offset distance. Larger offsets need proportionally larger tolerances.

## Key Files

| File | Size | Purpose |
|------|------|---------|
| `BRepOffset_MakeOffset.cxx` | ~350KB | Main algorithm |
| `BRepOffset_MakeOffset_1.cxx` | ~280KB | Continued implementation |
| `BRepOffset_Tool.cxx` | ~140KB | Utilities |
| `BRepOffset_Inter3d.cxx` | | 3D face-face intersection |
| `BRepOffset_Inter2d.cxx` | | 2D edge-edge intersection |
| `BRepOffset_Analyse.cxx` | | Edge classification |

## Architectural Limitations

1. **No global feasibility check** - Algorithm discovers failures during computation, not before.

2. **No geometry removal** - Cannot delete faces that become invalid and reconstruct topology.

3. **Local-to-global stitching** - Hopes that local face offsets + intersections produce globally valid result.

4. **Tolerance inflation** - Large offsets require large tolerances, degrading precision.

## What Would "Geometry Eating" Require?

A more robust offset would need:

1. **Pre-analysis**: Identify faces where offset > local min radius
2. **Face removal**: Delete those faces from consideration
3. **Boundary extension**: Extend adjacent faces to fill the gap
4. **Re-intersection**: Compute new edges where extended faces meet
5. **Topology reconstruction**: Build new shell with different face count

This is architecturally very different from OCCT's current approach.
