#!/bin/bash -x

WORKSPACE_DIR="/home/ilya/git/polyhedra-correction-library"
SOURCE_DIR=${WORKSPACE_DIR}
BUILD_DIR=${WORKSPACE_DIR}/buildRelease

${WORKSPACE_DIR}/buildRelease/Recoverer_test -r native -p l2 -z 0.1 -x -b -s -f poly-data-in/1_GIA/shadow_contours.dat -c poly-data-in/1_GIA/polyhedron.dat -o poly-data-out/1_GIA
