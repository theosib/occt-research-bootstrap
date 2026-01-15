# Lane map: meshing

Scope: triangulation / discretization of B-Rep shapes and related data model / tooling.

Source lane definition: `notes/maps/lanes.md` (entry packages: `BRepMesh`, `IMeshData`, `IMeshTools`).

## Package footprint (size proxy)

From `notes/maps/packages.json`:
- `BRepMesh`: 41 sources, 60 headers, 82 class/struct decls
- `IMeshData`: 9 sources, 13 headers, 18 class/struct decls
- `IMeshTools`: 9 sources, 11 headers, 14 class/struct decls

## Core types / entry points (with code citations)

- `occt/src/BRepMesh/BRepMesh_IncrementalMesh.hxx` — `BRepMesh_IncrementalMesh` (mesher entry point; supports parallel mode and parameter struct)
- `occt/src/IMeshTools/IMeshTools_Parameters.hxx` — `IMeshTools_Parameters` (meshing parameters: deflection/angle/min-size/surface controls)
- `occt/src/IMeshTools/IMeshTools_Context.hxx` — `IMeshTools_Context` (context carrying parameters + algorithm choices)

## Include graph evidence (who pulls these packages in)

Data source: `notes/maps/include_graph.core.dot` (heaviest edges summarized in `notes/maps/include_graph.core.md`).

Top inbound include edges into lane packages:
- `BRepMesh` -> `IMeshData`: 68
- `BRepMesh` -> `IMeshTools`: 35
- `BRepMeshData` -> `IMeshData`: 14
- `MeshTest` -> `BRepMesh`: 10

## Local dependency shape (what these packages depend on)

From `notes/maps/include_graph.core.dot` (largest direct edges originating in lane packages):
- `BRepMesh` -> `IMeshData`: 68
- `BRepMesh` -> `Standard`: 50
- `BRepMesh` -> `IMeshTools`: 35
- `BRepMesh` -> `gp`: 16
- `BRepMesh` -> `TopoDS`: 13
- `BRepMesh` -> `Precision`: 11
- `BRepMesh` -> `BRep`: 10
- `IMeshData` -> `NCollection`: 10
- `IMeshData` -> `TopoDS`: 8
- `IMeshTools` -> `Standard`: 13
- `IMeshTools` -> `IMeshData`: 5

## Robustness / guardrails (observed)

- `occt/src/BRepMesh/BRepMesh_IncrementalMesh.hxx` — `BRepMesh_IncrementalMesh::initParameters()` throws `Standard_NumericError` if `Deflection < Precision::Confusion()` or `Angle < Precision::Angular()`; and normalizes interior parameters (`DeflectionInterior`, `AngleInterior`) and `MinSize` using `IMeshTools_Parameters::RelMinSize()` and `Precision::Confusion()`.

## Suggested dossier entry points (next task)

If writing `task-8.2` (dossier), start from:
- `occt/src/BRepMesh/BRepMesh_IncrementalMesh.hxx` — parameter validation + `Perform()` entry points and status flags
- `occt/src/IMeshTools/IMeshTools_Parameters.hxx` — what each parameter controls (deflection, min size, surface deviation checks)
- `occt/src/IMeshData/*` — core mesh data structures (model representation and how triangulation is stored/updated)
