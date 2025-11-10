#!/usr/bin/env bash
set -euo pipefail

REPO_DIR=$(pwd)
IMAGE_TAG=ctds-build:latest
DOCKERFILE_DIR=docker

echo "[INFO] Repo: $REPO_DIR"

# Build docker image if missing
if ! docker image inspect "$IMAGE_TAG" >/dev/null 2>&1; then
  echo "[INFO] Building docker image $IMAGE_TAG from $DOCKERFILE_DIR..."
  docker build --platform linux/amd64 -t "$IMAGE_TAG" "$DOCKERFILE_DIR"
else
  echo "[INFO] Docker image $IMAGE_TAG already exists."
fi

# Ensure inside script is executable
chmod +x scripts/inside_run.sh

# Run the inside script in the container (simpler quoting)
docker run --platform linux/amd64 --rm -v "$REPO_DIR":/work -w /work "$IMAGE_TAG" bash /work/scripts/inside_run.sh

echo "[INFO] Done. If the container exited 0 then all tests passed."

echo "To run a single test interactively, you can:"
echo "  docker run --platform linux/amd64 --rm -it -v \"$REPO_DIR\":/work -w /work $IMAGE_TAG bash"
echo "  then inside container: ./c-tds test/<file>.ctds && gcc -o program output/<file>.s functions.c && ./program"
