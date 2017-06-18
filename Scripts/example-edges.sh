#!/bin/bash

MAX_CLUSTER_ERROR=1e-3 ANGLE_DIFF_LIMIT=0.1 EDGE_LENGTH_LIMIT=1.4 ./buildRelease/Recoverer_test -x -C -f ./poly-data-in/1_GIA/shadow_contours.dat -c ./poly-data-in/1_GIA/polyhedron.dat -o poly-data-out/1_GIA -v
