# Repro: lane-meshing

## Goal

Exercise OCCT meshing (`BRepMesh_IncrementalMesh`) deterministically and record mesh size proxies:

- per-deflection totals (faces with triangulation, nodes, triangles)
- mesher status flags

## Preconditions

- OCCT build exists and includes `TKMesh` (run `just occt-build` if needed).

## How to run (OCCT oracle)

From repo root:

- `just occt-build`
- `bash repros/lane-meshing/run.sh`

## Outputs

- Output files:
  - `repros/lane-meshing/golden/meshing.json`
- Match criteria:
  - exact: all strings, bools, integers (counts/flags)
  - tolerant: floating-point parameters (angle/deflection), compare within `eps = 1e-9`

## Scenarios covered / not covered

- Covered:
  - `BRepMesh_IncrementalMesh` on a box + cylinder with coarse vs finer deflection
  - deterministic mesh size proxies (nodes/triangles) and status flags
- Not covered (next extension):
  - per-face breakdowns, edge discretization details, and angular-deflection sensitivity sweeps
