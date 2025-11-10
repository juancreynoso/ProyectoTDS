#!/usr/bin/env bash
set -euo pipefail

# This script runs INSIDE the docker container. It assumes the repo is mounted at /work
cd /work

# timeout for each test program (seconds)
TIMEOUT=5

echo "[CONTAINER] make clean && make"
make clean || true
make

mkdir -p output/test_results
pass=0
fail=0

for f in test/*.ctds; do
  echo "=== TEST: $f ==="
  # run compiler to produce assembly
  if ! ./c-tds "$f"; then
    echo "  [ERROR] c-tds failed for $f"
    fail=$((fail+1))
    continue
  fi

  base=$(basename "$f" .ctds)
  sfile=output/${base}.s
  if [ ! -f "$sfile" ]; then
    echo "  [ERROR] assembly not generated: $sfile"
    fail=$((fail+1))
    continue
  fi

  outbin=output/program_${base}

  # compile the generated assembly and link with functions.c
  if ! gcc -o "$outbin" "$sfile" functions.c; then
    echo "  [ERROR] gcc/link failed for $sfile"
    fail=$((fail+1))
    continue
  fi

  # run the program with a timeout and capture output; assert() will print [OK]/[FAIL]
  set +e
  echo "  [RUN] $outbin (timeout ${TIMEOUT}s)"
  tmpout=$(mktemp)
  timeout ${TIMEOUT}s "$outbin" >"$tmpout" 2>&1
  rc=$?
  # print indented output
  sed "s/^/    /" "$tmpout"
  set -e

  if [ "$rc" -eq 124 ]; then
    echo "  [RESULT] TIMEOUT (>$TIMEOUT s)"
    fail=$((fail+1))
  else
    # if assert printed [FAIL] anywhere, test fails
    if grep -q "\[FAIL\]" "$tmpout"; then
      echo "  [RESULT] FAIL (assert reported failure)"
      fail=$((fail+1))
    else
      echo "  [RESULT] PASS"
      pass=$((pass+1))
    fi
  fi

  rm -f "$tmpout"

  echo
done

echo "SUMMARY: passed=$pass failed=$fail"

# propagate non-zero exit if any failed
if [ "$fail" -ne 0 ]; then
  exit 1
fi

exit 0
