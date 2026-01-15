# Repro: lane-topology-data-model

## Goal

Exercise OCCT’s topology data model (`TopoDS`/`TopLoc`/`TopAbs`/`TopTools`) in a deterministic way:

- Shape identity tiers: `IsPartner` vs `IsSame` vs `IsEqual`
- Location effects: same shared `TShape` instantiated at different `TopLoc_Location`
- Orientation effects: same `(TShape, Location)` with flipped `TopAbs_Orientation`
- Container semantics: `TopTools_IndexedMapOfShape` identity ignores orientation (uses `IsSame`)
- Traversal: vertex counts across per-instance shapes vs compound-of-instances

## Preconditions

- OCCT build exists and includes `TKBRep`, `TKTopAlgo`, `TKPrim` (run `just occt-build` if needed).

## How to run (OCCT oracle)

From repo root:

- `just occt-build`
- `bash repros/lane-topology-data-model/run.sh`

## Outputs

- Output files:
  - `repros/lane-topology-data-model/golden/topology-data-model.json`
- Match criteria:
  - exact: all strings, bools, integers (counts), and enum-like fields
  - tolerant: all floating-point fields (points/translations), compare within `eps = 1e-9`

## Scenarios covered / not covered

- Covered:
  - `TopoDS_Shape` identity tiers (`IsPartner`/`IsSame`/`IsEqual`) across location + orientation changes
  - `TopTools_IndexedMapOfShape` semantics (orientation ignored, location matters)
  - traversal differences for a compound of two instances
- Not covered (next extension):
  - `TopLoc_Location` datum-chain cancellation/inversion edge-cases
  - exceptions for negative/non-unit scaling via `TopoDS_Shape::Location(..., raiseExc)`
