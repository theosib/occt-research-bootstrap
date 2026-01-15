# Lane map: core-kernel

Scope: foundational types, math, geometry primitives and curves/surfaces.

Source lane definition: `notes/maps/lanes.md` (entry packages: `Standard`, `NCollection`, `math`, `gp`, `Geom`, `Geom2d`).

## Provenance (required)

- OCCT version + build config: `notes/maps/provenance.md`
- Map inputs:
  - `notes/maps/packages.json`
  - `notes/maps/include_graph.core.dot` / `notes/maps/include_graph.core.md`

## Package footprint (size proxy)

From `notes/maps/packages.json`:
- `Standard`: 20 sources, 78 headers, 55 class/struct decls
- `NCollection`: 13 sources, 62 headers, 84 class/struct decls
- `math`: 43 sources, 55 headers, 52 class/struct decls
- `gp`: 36 sources, 44 headers, 42 class/struct decls
- `Geom`: 40 sources, 42 headers, 58 class/struct decls
- `Geom2d`: 21 sources, 22 headers, 33 class/struct decls

## Core types / entry points (with code citations)

- `occt/src/gp/gp_Pnt.hxx` — `gp_Pnt` (3D point primitive)
- `occt/src/Geom/Geom_BSplineCurve.hxx` — `Geom_BSplineCurve` (B-spline curve definition; control points/knots/weights)
- `occt/src/math/math.hxx` — `math` (numerical utilities; quadrature tables like Gauss/Kronrod)

## Include graph evidence (who pulls these packages in)

Data source: `notes/maps/include_graph.core.dot` (heaviest edges summarized in `notes/maps/include_graph.core.md`).

Top inbound include edges into lane packages (high fan-in hotspots):
- `HLRBRep` -> `Standard`: 238
- `Extrema` -> `Standard`: 229
- `GeomFill` -> `Standard`: 223
- `Geom` -> `Standard`: 209
- `GccAna` -> `gp`: 204
- `math` -> `Standard`: 192
- `Geom` -> `gp`: 185
- `Extrema` -> `gp`: 183
- `Geom2d` -> `Standard`: 111
- `gp` -> `Standard`: 97

## Local dependency shape (what these packages depend on)

From `notes/maps/include_graph.core.dot` (largest direct edges originating in lane packages):
- `Geom` -> `Standard`: 209
- `math` -> `Standard`: 192
- `Geom` -> `gp`: 185
- `Geom2d` -> `Standard`: 111
- `gp` -> `Standard`: 97
- `Geom2d` -> `gp`: 91
- `NCollection` -> `Standard`: 74

## Suggested dossier entry points (next task)

If writing `task-3.2` (dossier), start from:
- `occt/src/gp/gp_Pnt.hxx` and `occt/src/gp/gp_XYZ.hxx` — primitive geometry types and invariants
- `occt/src/Geom/Geom_BSplineCurve.hxx` — what “valid” knots/multiplicities/poles mean (construction constraints and continuity rules)
- `occt/src/Geom2d/*` — 2D curve counterparts (for pcurves and parameter-space operations)
- `occt/src/math/math.hxx` + `occt/src/math/*` — where numerical tolerances and table-driven algorithms enter (quadrature, solvers)
