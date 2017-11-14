#!/bin/bash -x

WORKSPACE_DIR="/home/ilya/git/polyhedra-correction-library"
SOURCE_DIR=${WORKSPACE_DIR}
BUILD_DIR=${WORKSPACE_DIR}/build-windows

if [ ! -d ${BUILD_DIR} ]
then
	mkdir ${BUILD_DIR}
fi

cd ${BUILD_DIR}

if [ ! -f Makefile ]
then
	cmake ${SOURCE_DIR} -DCMAKE_SYSTEM_NAME="Windows" -DCMAKE_C_COMPILER="x86_64-w64-mingw32-gcc" -DCMAKE_CXX_COMPILER="x86_64-w64-mingw32-g++" -DCMAKE_RC_COMPILER=`which x86_64-w64-mingw32-windres` -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER -DCMAKE_BUILD_TYPE=Release -DENABLE_IPOPT=On -DENABLE_CPLEX=On -DENABLE_CLP=On -DENABLE_GLPK=On -DIPOPT_INSTALL_PREFIX="/home/ilya/git/Ipopt-3.12.1.gcc/build-2/"
fi

if [ -f Makefile ]
then
	make -j4 VERBOSE=1
else
	echo "No Makefile found!"
fi
