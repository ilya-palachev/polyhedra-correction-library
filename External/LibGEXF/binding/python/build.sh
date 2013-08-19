#!/bin/bash
echo " ---------------------------------"
echo " |        Python building        |"
echo " ---------------------------------"
echo ""
echo "Generating interface files.."
swig -c++ -python -fvirtual -o libgexf_wrap.cpp libgexf.i

echo "Compiling.."
python setup.py build

echo "Cleaning.."
./clean.sh

echo "Done!"

