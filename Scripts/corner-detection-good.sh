#!/bin/bash

export LD_LIBRARY_PATH="/home/ilya/thesis/Ipopt/install/release/lib"
PCL_PATH="/home/ilya/thesis/polyhedra-correction-library/"

"${PCL_PATH}""/buildRelease/calculateHorizontalSection" "poly-data-in/1_GIA/polyhedron.dat" -5 | tee model.txt
cat model.txt | grep "^    x = " | sed -e "s/    x = //g;s/, y =//g" > model-plot.txt

"${PCL_PATH}""/buildRelease/detectSectionCorners" "poly-data-in/1_GIA/shadow_contours.dat" -5 10 20. 4 0.5 1 | tee estimate.txt
cat estimate.txt | grep "^    x = " | sed -e "s/    x = //g;s/, y =//g" > estimate-plot.txt

gnuplot -persist -e "plot 'model-plot.txt' using 1:2 with lines, 'estimate-plot.txt' using 1:2 with lines;"
