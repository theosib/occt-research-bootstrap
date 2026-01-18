# Fillet and Chamfer Algorithm (ChFi3d)

OCCT version: 7.9.3
Packages: `ChFi3d`, `ChFiDS`, `ChFiKPart`, `BRepFilletAPI`

## Overview

OCCT's fillet/chamfer algorithm creates rounded (fillet) or flat (chamfer) transitions between adjacent faces along selected edges. The algorithm:

1. Propagates along tangent edge chains to form "spines"
2. Walks along each spine, generating blend surface by maintaining contact with both faces
3. Handles corners where multiple spines meet
4. Reconstructs topology

The fundamental weakness: the spine is not C2-continuous, but the walking algorithm expects smooth guidance.

## Entry Points

```cpp
// Fillet
BRepFilletAPI_MakeFillet fillet(shape);
fillet.Add(radius, edge);           // Constant radius
fillet.Add(r1, r2, edge);          // Evolving radius
fillet.Build();
TopoDS_Shape result = fillet.Shape();

// Chamfer
BRepFilletAPI_MakeChamfer chamfer(shape);
chamfer.Add(dist, edge);           // Symmetric
chamfer.Add(d1, d2, edge, face);   // Asymmetric (d1 on face)
chamfer.Build();
```

These delegate to `ChFi3d_FilBuilder` (fillet) or `ChFi3d_ChBuilder` (chamfer), both inheriting from `ChFi3d_Builder`.

## Core Data Structures

### Spine (`ChFiDS_Spine`)

A spine is the "guideline" along which the blend surface is generated:

```cpp
class ChFiDS_Spine {
  // Sequence of edges forming the spine
  TopTools_SequenceOfShape spine;

  // Elementary splines with C2 info (claimed but not actually C2)
  ChFiDS_ListOfHElSpine elspines;

  // Boundary conditions at endpoints
  ChFiDS_State firstState, lastState;  // Free, Closed, BreakPoint

  // Error tracking
  ChFiDS_ErrorStatus errorStatus;
};
```

**Critical comment from the source:**
> "guideline represented is not C2, although the path claims it"

This mismatch between claimed and actual continuity causes walking failures.

### Stripe (`ChFiDS_Stripe`)

A stripe is a complete blend band along a spine:

```cpp
class ChFiDS_Stripe {
  Handle(ChFiDS_Spine) mySpine;        // The guideline
  ChFiDS_SequenceOfSurfData myHdata;   // Surface patches

  // Supporting faces
  Standard_Integer indexOfS1, indexOfS2;

  // Orientation info
  TopAbs_Orientation orientationOnFace1, orientationOnFace2;
};
```

### SurfData (`ChFiDS_SurfData`)

Individual surface patch along a stripe:

```cpp
class ChFiDS_SurfData {
  // The blend surface
  Handle(Geom_Surface) surf;

  // Interference curves on each supporting face
  ChFiDS_FaceInterference intf1, intf2;  // UV curves

  // Start/end points with tolerance
  ChFiDS_CommonPoint firstOnS1, lastOnS1;
  ChFiDS_CommonPoint firstOnS2, lastOnS2;

  // Twist detection
  Standard_Boolean twistOnS1, twistOnS2;
};
```

## Algorithm Phases

### Phase 1: Spine Creation and Propagation

When `Add(edge)` is called:

```
1. Find the two faces adjacent to edge
2. Look for tangent-continuous edges at each vertex
3. Propagate along tangent chains
4. Create spine containing all edges in chain
```

Tangency is determined by face adjacency and edge direction continuity.

### Phase 2: Surface Generation (Walking)

For each spine, the algorithm "walks" along it, computing the blend surface:

```cpp
// Simplified view of the walking loop
PerformSurf(Stripe, Spine) {
  for (each position along spine) {
    // Find point where sphere of radius R touches both faces
    // This is a nonlinear solve
    ComputeData(pos, face1, face2, radius, ...);

    // Step forward along spine
    IntWalk_PWalking::Perform(...);
  }
}
```

The walking algorithm (`IntWalk_PWalking`) traces through parameter space, maintaining:
- Tangency with face 1
- Tangency with face 2
- Distance = radius from spine

Parameters controlling the walk:
```cpp
Standard_Real Fleche = radius * 0.05;  // Max deflection
Standard_Real MaxStep;                  // Step size limit
```

### Phase 3: Special Cases (KPart)

`ChFiKPart_ComputeData` detects analytically-solvable cases:

| Case | Surfaces | Solution |
|------|----------|----------|
| Plane-Plane | Two planes | Cylindrical fillet (exact) |
| Plane-Cylinder | Plane + cylinder | Toric section (exact) |
| Cylinder-Cylinder | Two cylinders | Torus (exact) |
| Plane-Cone | Plane + cone | Special analytic |

These use closed-form formulas instead of walking → much more robust.

### Phase 4: Corner Handling

When spines meet at a vertex:

**Two-corner** (2 stripes meeting):
```cpp
PerformTwoCorner(vertex, stripe1, stripe2) {
  // Intersect the two blend surfaces
  // Create connecting geometry
  // Update topology
}
```

**Three-corner** (3+ stripes meeting):
```cpp
PerformMoreThreeCorner(vertex, stripes...) {
  // Use GeomPlate to interpolate constraints
  // Fit a surface through all stripe endpoints
  // This often fails for complex corners
}
```

The code uses `GeomPlate_BuildPlateSurface` which solves an overdetermined constraint system. Failure here leaves a hole in the result.

### Phase 5: Topology Reconstruction

Build the final shape:
- Create faces from SurfData surfaces
- Connect to trimmed original faces
- Handle regularities (C1/C2 continuity flags)

## Error Status

```cpp
enum ChFiDS_ErrorStatus {
  ChFiDS_Ok,              // Success
  ChFiDS_Error,           // Generic failure
  ChFiDS_WalkingFailure,  // Walking algorithm diverged
  ChFiDS_StartsolFailure, // Couldn't find initial solution
  ChFiDS_TwistedSurface   // Generated surface is self-intersecting
};
```

## Why Fillets Fail: Detailed Analysis

### 1. StartsolFailure: Radius Too Large

The walking algorithm needs a starting point where a sphere of radius R touches both faces. If R is too large:

```
Face 1         Face 2
  |             |
  |   No sphere |
  |   of radius |
  |   R fits    |
  |_____________|
       ↑
    R > gap/2
```

No valid starting configuration exists → immediate failure.

### 2. WalkingFailure: Divergence During Tracing

The walking algorithm can diverge when:

- **High curvature**: Surface curves faster than step size accounts for
- **Near singularity**: Approaching apex of cone, axis of cylinder
- **Spine discontinuity**: The non-C2 spine causes sudden direction changes

```
Spine (not C2)
    ____/\____
        ↑
    Curvature discontinuity
    Walking algorithm "surprised"
```

### 3. TwistedSurface: Self-Intersection

When parameters are marginally valid, the generated surface may fold back on itself:

```
Expected:     Actual (twisted):
   ___           ___
  /   \         /   \
 |     |       |  X  |  ← surface crosses itself
  \___/         \↗↘/
```

Detected via `twistOnS1`/`twistOnS2` flags but not recoverable.

### 4. Corner Failures: Constraint Incompatibility

At 3+ corners, `GeomPlate` must satisfy:
- Continuity with stripe 1 endpoint
- Continuity with stripe 2 endpoint
- Continuity with stripe 3 endpoint
- Smoothness constraints

These constraints may be geometrically incompatible → solver fails or produces garbage.

### 5. Explicitly Unhandled Cases

From the documentation:
> "The following cases are not handled:
> - The end point of the contour is the intersection of 4 or more edges
> - The intersection of the fillet with a face which limits the contour is not fully contained in this face"

These produce partial results: `HasResult() == true` but with holes.

## The Fundamental Architecture Problem

The code explicitly acknowledges:

```cpp
// Comment in ChFiDS_Spine.hxx:
// "guideline represented is not C2, although the path claims it"
```

The walking algorithm assumes C2 continuity for stable stepping. The spine (concatenated edges) is at best C1 at edge junctions. This mismatch causes:

1. Sudden direction changes at edge boundaries
2. Step size miscalculation
3. Accumulated error across multiple edges

A proper fix would require either:
- Making the spine actually C2 (reparameterization with blending)
- Rewriting the walker to handle C1 discontinuities explicitly

## Tolerance Chain

Tolerance flows through multiple layers:

```
Input shape tolerance
    ↓
Spine construction tolerance (tolesp)
    ↓
Walking step tolerance (Fleche, MaxStep)
    ↓
Surface generation tolerance
    ↓
Output shape tolerance (inflated)
```

Each stage can amplify errors. The final tolerance is often significantly larger than the input.

## Key Files

| File | Lines | Purpose |
|------|-------|---------|
| `ChFi3d_Builder.cxx` | ~2000 | Core algorithm orchestration |
| `ChFi3d_Builder_0.cxx` | ~3500 | Initialization, analysis |
| `ChFi3d_Builder_2.cxx` | ~3000 | Surface walking |
| `ChFi3d_Builder_CnCrn.cxx` | ~3500 | Corner handling (2-corner, 3-corner) |
| `ChFi3d_FilBuilder.cxx` | ~1500 | Fillet-specific surface generation |
| `ChFi3d_ChBuilder.cxx` | ~800 | Chamfer-specific |
| `ChFiKPart_ComputeData.cxx` | ~2000 | Analytic special cases |

## What Would Make Fillets More Robust?

1. **Pre-validation**: Check if radius is geometrically feasible before attempting
2. **C2 spine**: Reparameterize edge chains with smooth blending at junctions
3. **Adaptive stepping**: Detect curvature and reduce step size proactively
4. **Better corner handling**: More sophisticated constraint solving for 3+ corners
5. **Partial success**: Return valid portions even when some edges fail

Current OCCT attempts some of these (KPart for special cases, error codes for diagnosis) but the fundamental spine continuity issue remains unaddressed.
