#!/bin/bash

NOISE_VARIANCE=0.01 GAUGE_MODE=1 RANGE_MODE=1 LD_LIBRARY_PATH="/home/ilya/thesis/Ipopt/install/release/lib"    ./buildRelease/fitTractableSet 1000 2>&1 | grep RESULT
