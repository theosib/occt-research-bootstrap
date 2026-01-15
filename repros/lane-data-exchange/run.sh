#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
REPRO_DIR="$ROOT/repros/lane-data-exchange"

# build-occt/env.sh assumes unset variables may be referenced; avoid nounset failures.
set +u
source "$ROOT/build-occt/env.sh" i
set -u

mkdir -p "$REPRO_DIR/build" "$REPRO_DIR/golden"

cxx="${CXX:-g++}"
"$cxx" -std=c++17 -O2 -g \
  -I"$CSF_OCCTIncludePath" \
  "$REPRO_DIR/data_exchange.cpp" \
  -L"$CSF_OCCTLibPath" \
  -lTKDESTEP -lTKXSBase -lTKTopAlgo -lTKBRep -lTKPrim -lTKGeomBase -lTKG3d -lTKG2d -lTKMath -lTKernel \
  -o "$REPRO_DIR/build/data-exchange"

step_path="$REPRO_DIR/build/roundtrip.step"
"$REPRO_DIR/build/data-exchange" "$step_path" | sed -n '/^{/,$p' > "$REPRO_DIR/golden/data-exchange.json"
echo "wrote $REPRO_DIR/golden/data-exchange.json (step at $step_path)"
