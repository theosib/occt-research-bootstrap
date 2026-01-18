# Claude Notes

Deep-dive analysis of OCCT internals, focusing on understanding algorithms and their failure modes.

These notes aim to explain *how* algorithms work, not just *what* exists.

## Index

- [3D Offset (BRepOffset)](offset-algorithm.md) - Face-by-face offset with intersection stitching
- [Fillets and Chamfers (ChFi3d)](fillet-algorithm.md) - Spine-based surface walking

## Context

FreeCAD developers frequently encounter OCCT failures in:
- 3D offset operations (especially when offset > local curvature radius)
- Fillet/chamfer operations (corner cases, large radii)

Understanding the algorithms helps diagnose why failures occur and what (if anything) can be done.
