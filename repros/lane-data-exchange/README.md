# Repro: lane-data-exchange

## Goal

Exercise OCCT data exchange deterministically via a STEP roundtrip:

- export a small compound shape to STEP
- re-import it and transfer roots
- record read/write statuses, STEP model entity count, and imported topology counts/bbox

## Preconditions

- OCCT build exists and includes STEP libs (`TKDESTEP`, `TKXSBase`) (run `just occt-build` if needed).

## How to run (OCCT oracle)

From repo root:

- `just occt-build`
- `bash repros/lane-data-exchange/run.sh`

## Outputs

- Output files:
  - `repros/lane-data-exchange/golden/data-exchange.json`
- Match criteria:
  - exact: all strings, bools, integers (status codes, counts)
  - tolerant: all floating-point fields (bbox), compare within `eps = 1e-9`

## Scenarios covered / not covered

- Covered:
  - STEP write+read+transfer of a simple compound (box + translated cylinder)
  - entity counts and topology count/bbox stability after import
- Not covered (next extension):
  - AP selection (203/214/242), assembly/product structure, colors/names, and tolerance/units nuances

