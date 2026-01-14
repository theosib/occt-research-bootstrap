# Dossier: <TITLE>

Status: draft

## Purpose

What problem/area this dossier covers and why it matters.

## High-level pipeline

Describe the main phases and data flow at a subsystem level.

## Key classes/files

List the key entry points and supporting types with citations:
- `path/to/file.hxx` — `ClassName::MethodName` (role)

## Core data structures + invariants

What structures appear central, and what invariants you infer from the code:
- Structure: `TypeName` (where)
  - Invariants: …

## Tolerance / robustness behaviors (observed)

Record any tolerance values, fuzzy comparisons, fallback paths, and error handling patterns you see:
- `path/to/file.cxx` — `FunctionName` uses `<tolerance>` for …

## Runnable repro (optional)

One minimal runnable reproduction (if needed) under `tools/repros/`:
- Path: `tools/repros/<slug>/README.md`
- How to run: …
- Expected output: …

## Compare to papers / alternatives

Briefly compare to at least 2-3 alternative approaches (papers, libraries, or strategies) and tradeoffs:
- Alternative A: …
- Alternative B: …
