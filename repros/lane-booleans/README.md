# Repro: lane-booleans

## Goal

Exercise OCCT boolean operations deterministically and record stable, machine-checkable outputs:

- `BRepAlgoAPI_Fuse` / `BRepAlgoAPI_Common` / `BRepAlgoAPI_Cut`
- Error/warning presence (`HasErrors`/`HasWarnings`)
- Result validity (`BRepCheck_Analyzer::IsValid`)
- Result topology counts (solids/faces/edges/vertices) and bounding box

## Preconditions

- OCCT build exists and includes boolean libs (`TKBO`, `TKBool`) (run `just occt-build` if needed).

## How to run (OCCT oracle)

From repo root:

- `just occt-build`
- `bash repros/lane-booleans/run.sh`

## Outputs

- Output files:
  - `repros/lane-booleans/golden/booleans.json`
- Match criteria:
  - exact: all strings, bools, integers (counts)
  - tolerant: all floating-point fields (bbox), compare within `eps = 1e-9`

## Scenarios covered / not covered

- Covered:
  - overlapping solid/solid booleans (FUSE/COMMON/CUT) with parallel disabled
  - validity + basic topology counts + bbox sanity checks
- Not covered (next extension):
  - fuzzy booleans (`SetFuzzyValue`) and tolerance-driven merges
  - non-solid operands (faces/shells), SECTION/SPLITTER pipelines
