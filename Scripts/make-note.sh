#!/bin/bash -x

WORKSPACE_DIR="/home/ilya/thesis/polyhedra-correction-library"
SOURCE_DIR=${WORKSPACE_DIR}
BUILD_DIR=${WORKSPACE_DIR}/buildRelease

if [ ! -d ${BUILD_DIR} ]
then
	mkdir ${BUILD_DIR}
fi

cd ${BUILD_DIR}

if [ ! -f Makefile ]
then
	cmake ${SOURCE_DIR} -DCMAKE_BUILD_TYPE="Release" -DENABLE_IPOPT=On -DIPOPT_INSTALL_PREFIX="/home/ilya/thesis/polyhedra-correction-library/Dependencies/Ipopt/install/" 
fi

if [ -f Makefile ]
then
	make -j6
else
	echo "No Makefile found!"
fi
