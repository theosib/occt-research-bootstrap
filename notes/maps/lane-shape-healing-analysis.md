# Lane map: shape-healing-analysis

Scope: shape validation, fixing, and upgrades; common entry points when dealing with imperfect input geometry/topology.

Source lane definition: `notes/maps/lanes.md` (entry packages: `ShapeFix`, `ShapeAnalysis`, `ShapeUpgrade`).

## Provenance (required)

- OCCT version + build config: `notes/maps/provenance.md`
- Map inputs:
  - `notes/maps/packages.json`
  - `notes/maps/include_graph.core.dot` / `notes/maps/include_graph.core.md`

## Package footprint (size proxy)

From `notes/maps/packages.json`:
- `ShapeFix`: 23 sources, 25 headers, 45 class/struct decls
- `ShapeAnalysis`: 19 sources, 23 headers, 45 class/struct decls
- `ShapeUpgrade`: 33 sources, 33 headers, 48 class/struct decls

## Core types / entry points (with code citations)

- `occt/src/ShapeFix/ShapeFix_Shape.hxx` — `ShapeFix_Shape` (general “fix the shape” façade; delegates to per-entity fix tools)
- `occt/src/ShapeAnalysis/ShapeAnalysis_ShapeTolerance.hxx` — `ShapeAnalysis_ShapeTolerance` (compute/min/max/avg tolerances; query shapes over/within thresholds)
- `occt/src/ShapeUpgrade/ShapeUpgrade_ShapeDivide.hxx` — `ShapeUpgrade_ShapeDivide` (divide/split faces in shells; precision/tolerance controls; status reporting)

## Include graph evidence (who pulls these packages in)

Data source: `notes/maps/include_graph.core.dot` (heaviest edges summarized in `notes/maps/include_graph.core.md`).

Top inbound include edges into lane packages:
- `ShapeFix` -> `ShapeAnalysis`: 48
- `ShapeUpgrade` -> `ShapeAnalysis`: 19
- `SWDRAW` -> `ShapeAnalysis`: 13
- `SWDRAW` -> `ShapeFix`: 11
- `SWDRAW` -> `ShapeUpgrade`: 11

## Local dependency shape (what these packages depend on)

From `notes/maps/include_graph.core.dot` (largest direct edges originating in lane packages):
- `ShapeFix` -> `TopoDS`: 139
- `ShapeUpgrade` -> `Standard`: 134
- `ShapeFix` -> `Standard`: 93
- `ShapeAnalysis` -> `TopoDS`: 85
- `ShapeAnalysis` -> `Standard`: 77
- `ShapeUpgrade` -> `TopoDS`: 76
- `ShapeUpgrade` -> `Geom`: 76
- `ShapeFix` -> `TopTools`: 63
- `ShapeFix` -> `BRep`: 44
- `ShapeAnalysis` -> `BRep`: 32

## Suggested dossier entry points (next task)

If writing `task-7.2` (dossier), start from:
- `occt/src/ShapeFix/ShapeFix_Shape.hxx` (+ associated fix tools like `ShapeFix_Face`, `ShapeFix_Wire`, etc.) — what “fixing” means and how status flags are reported
- `occt/src/ShapeAnalysis/ShapeAnalysis_ShapeTolerance.hxx` — tolerance inspection patterns and how they inform downstream repair decisions
- `occt/src/ShapeUpgrade/ShapeUpgrade_ShapeDivide.hxx` — which upgrades are tolerance/precision-driven and how shape modifications are recorded (`GetContext()`)
