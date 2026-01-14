# Dossier task prompt (OCCT)

Use this prompt when working on a **type:dossier** Backlog task.

Rules:
- Do not modify anything under `occt/`.
- Keep research outputs in:
  - `notes/dossiers/` (the dossier itself)
  - `tools/` (optional repro scripts under `tools/repros/...`)
- When writing notes, cite file paths and class/function names used.
- Prefer using MCP tools:
  - Backlog MCP: view/update tasks, add implementation plan, update acceptance criteria.
  - occt-lsp MCP: symbol lookup (definition/references/hover) to validate interpretations.

Workflow:
1) Read the task, then add an “Implementation plan” section to the task via Backlog MCP.
2) Identify the entry points (classes/functions) and locate their definitions and key call paths.
3) Summarize the high-level pipeline (phases + data flow).
4) Extract core data structures + invariants (as inferred from code).
5) Note tolerance/robustness behaviors observed (epsilons, fuzzy tolerances, fallback paths, error handling).
6) Write `notes/dossiers/<slug>.md` using `notes/dossiers/_template.md` as the structure.
7) If a runnable repro is required, add it under `tools/repros/<slug>/` (README + run steps).
8) Summarize findings into the task notes (short, actionable bullets).

Acceptance criteria checklist (suggested):
- [ ] Dossier exists/updated under `notes/dossiers/`
- [ ] Claims cite file paths and class/function names
- [ ] Task plan added before changes
- [ ] Optional repro exists under `tools/repros/...` (if requested)
