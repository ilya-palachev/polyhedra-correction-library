#!/bin/bash -x

WORKSPACE_DIR="/home/ilya/thesis/polyhedra-correction-library"
SOURCE_DIR="${WORKSPACE_DIR}"
BUILD_DIR="${WORKSPACE_DIR}/buildRelease"

LD_LIBRARY_PATH="/home/ilya/thesis/Ipopt/install/release/lib" "${WORKSPACE_DIR}/buildRelease/Recoverer_test" -f poly-data-in/1_GIA/shadow_contours.dat -o poly-data-out/1_GIA -r ipopt -p l2 -x -s -b -h
