---
id: task-1
title: Lane selection + initial task generation
status: Done
assignee: []
created_date: '2025-12-25 03:32'
updated_date: '2026-01-15 00:02'
labels: []
dependencies: []
---

## Description

<!-- SECTION:DESCRIPTION:BEGIN -->
Generate the initial lane list and seed the backlog with lane map+dossier tasks per `prompts/backlog/task_generation.md`.

Primary artifact:
- `notes/maps/lanes.md` (lane list + entry packages, derived from existing maps)

This task is responsible for creating the initial Backlog.md tasks (map + dossier) and milestones/labels/dependencies that reference that lane list.
<!-- SECTION:DESCRIPTION:END -->

## Acceptance Criteria
<!-- AC:BEGIN -->
- [x] #1 `notes/maps/lanes.md` exists and lists lanes with 1–2 sentence descriptions and 3–5 entry packages each, citing at least one supporting artifact under `notes/maps/` per lane.
- [x] #2 Backlog has one milestone (or parent task) per lane and exactly two tasks per lane: `type:map` and `type:dossier`.
- [x] #3 Each `type:dossier` task depends on its corresponding `type:map` task.
- [x] #4 All lane tasks reference `prompts/backlog/map_task.md` or `prompts/backlog/dossier_task.md` instead of duplicating workflow instructions.
- [x] #5 All tasks are labeled with `lane:<slug>` and `type:<map|dossier>`.
<!-- AC:END -->

## Implementation Plan

<!-- SECTION:PLAN:BEGIN -->
1) Confirm/refresh notes/maps/lanes.md against notes/maps/packages.md + include graphs (core + exchange/vis).
2) Create a parent task Lane: <slug> per lane.
3) For each lane, create two child tasks: Map + Dossier, with labels lane:<slug> + type:<map|dossier>, and make the dossier depend on the map.
4) Update task-1 notes with the created task IDs.
5) Mark task-1 Done once dependencies/labels are correct.
<!-- SECTION:PLAN:END -->

## Implementation Notes

<!-- SECTION:NOTES:BEGIN -->
Lane tasks created (parent -> map -> dossier):
- core-kernel: task-3 -> task-3.1 -> task-3.2
- topology-data-model: task-4 -> task-4.1 -> task-4.2
- brep-geometry-bridge: task-5 -> task-5.1 -> task-5.2
- booleans: task-6 -> task-6.1 -> task-6.2
- shape-healing-analysis: task-7 -> task-7.1 -> task-7.2
- meshing: task-8 -> task-8.1 -> task-8.2
- data-exchange: task-9 -> task-9.1 -> task-9.2
- visualization: task-10 -> task-10.1 -> task-10.2
<!-- SECTION:NOTES:END -->
