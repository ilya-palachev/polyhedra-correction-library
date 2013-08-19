#!/bin/bash
echo " ---------------------------------"
echo " |          All building         |"
echo " ---------------------------------"
echo ""

root=`pwd`

echo ""
cd $root/python
./build.sh

echo ""
cd $root/perl/Graph-LibGEXF
./build.sh

echo ""
cd $root/java
./build.sh

echo ""
cd $root/php
./build.sh

