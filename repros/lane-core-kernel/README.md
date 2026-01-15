# Repro: lane-core-kernel

## Goal

Exercise OCCT “core kernel” fundamentals in a deterministic way: version/tolerance constants, `gp` transforms, handle/refcount semantics (`Standard_Transient` + `Handle()`), and basic `NCollection` container behavior.

## Preconditions

- OCCT build exists and includes `TKernel` and `TKMath` (run `just occt-build` if needed).

## How to run (OCCT oracle)

From repo root:

- `just occt-build`
- `bash repros/lane-core-kernel/run.sh`

## Outputs

- Output files:
  - `repros/lane-core-kernel/golden/core-kernel.json`
- Match criteria:
  - exact: all strings, bools, integers (sizes/counts), and enum-like fields
  - tolerant: all floating-point fields, compare within `eps = 1e-9`

## Scenarios covered / not covered

- Covered:
  - version/tolerance constants, `gp` transform math, intrusive `Handle()` refcount, basic `NCollection` iteration
- Not covered (next extension):
  - exception/error-path behaviors (e.g., `Standard_DomainError` cases)
  - more container types (`DataMap`, `IndexedMap`) and allocator behaviors
