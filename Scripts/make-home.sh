#!/bin/bash -x

WORKSPACE_DIR="/home/ilya/git/polyhedra-correction-library"
SOURCE_DIR=${WORKSPACE_DIR}
BUILD_DIR=${WORKSPACE_DIR}/buildRelease

if [ ! -d ${BUILD_DIR} ]
then
	mkdir ${BUILD_DIR}
fi

cd ${BUILD_DIR}

if [ ! -f Makefile ]
then
	cmake ${SOURCE_DIR} -DCMAKE_BUILD_TYPE=Release -DENABLE_IPOPT=On -DENABLE_CPLEX=On -DENABLE_CLP=On -DENABLE_GLPK=On -DIPOPT_INSTALL_PREFIX="/home/ilya/git/Ipopt-3.12.1.gcc/build-2/"
fi

if [ -f Makefile ]
then
	make -j4
else
	echo "No Makefile found!"
fi
