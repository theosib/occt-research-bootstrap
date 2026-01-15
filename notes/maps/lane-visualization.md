# Lane map: visualization

Scope: interactive visualization stack and rendering backends.

Source lane definition: `notes/maps/lanes.md` (entry packages: `AIS`, `Prs3d`, `Graphic3d`, `OpenGl`, `V3d`, `Select3D`).

## Provenance (required)

- OCCT version + build config: `notes/maps/provenance.md`
- Map inputs:
  - `notes/maps/packages.json`
  - `notes/maps/include_graph.exchange_vis.dot` / `notes/maps/include_graph.exchange_vis.md`

## Package footprint (size proxy)

From `notes/maps/packages.json`:
- `Graphic3d`: 63 sources, 145 headers, 113 class/struct decls
- `OpenGl`: 53 sources, 100 headers, 150 class/struct decls
- `AIS`: 42 sources, 95 headers, 101 class/struct decls
- `Prs3d`: 22 sources, 41 headers, 32 class/struct decls
- `Select3D`: 17 sources, 25 headers, 27 class/struct decls
- `V3d`: 12 sources, 27 headers, 25 class/struct decls

## Core types / entry points (with code citations)

- `occt/src/AIS/AIS_Shape.hxx` — `AIS_Shape` (main interactive object for displaying shapes; selection/decomposition support)
- `occt/src/V3d/V3d_View.hxx` — `V3d_View` (view object; window binding, redraw, navigation)
- `occt/src/Graphic3d/Graphic3d_GraphicDriver.hxx` — `Graphic3d_GraphicDriver` (abstract driver interface)
- `occt/src/OpenGl/OpenGl_GraphicDriver.hxx` — `OpenGl_GraphicDriver` (OpenGL implementation of `Graphic3d_GraphicDriver`)

## Include graph evidence (exchange/vis filtered view)

Data source: `notes/maps/include_graph.exchange_vis.dot` (heaviest edges summarized in `notes/maps/include_graph.exchange_vis.md`).

Top inbound include edges into lane packages:
- `DsgPrs` -> `Prs3d`: 154
- `AIS` -> `Prs3d`: 108
- `AIS` -> `Graphic3d`: 93
- `OpenGl` -> `Graphic3d`: 84
- `StdPrs` -> `Prs3d`: 77
- `V3d` -> `Graphic3d`: 46
- `AIS` -> `Select3D`: 33

## Local dependency shape (what these packages depend on)

From `notes/maps/include_graph.exchange_vis.dot` (largest direct edges originating in lane packages):
- `AIS` -> `Prs3d`: 108
- `AIS` -> `Graphic3d`: 93
- `OpenGl` -> `Graphic3d`: 84
- `V3d` -> `Graphic3d`: 46
- `Prs3d` -> `Graphic3d`: 39
- `AIS` -> `Select3D`: 33

## Suggested dossier entry points (next task)

If writing `task-10.2` (dossier), start from:
- `occt/src/AIS/AIS_InteractiveContext.hxx` + `occt/src/AIS/AIS_Shape.hxx` — interaction model, selection modes, and presentation building
- `occt/src/V3d/V3d_View.hxx` + `occt/src/V3d/V3d_Viewer.hxx` — view lifecycle, redraw, window/backbuffer integration
- `occt/src/Graphic3d/Graphic3d_GraphicDriver.hxx` + `occt/src/OpenGl/OpenGl_GraphicDriver.hxx` — driver abstraction boundary and OpenGL backend responsibilities
