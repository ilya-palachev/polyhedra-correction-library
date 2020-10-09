#!/bin/bash -x

WORKSPACE_DIR="/home/ilya/thesis/polyhedra-correction-library"
SOURCE_DIR=${WORKSPACE_DIR}
BUILD_DIR=${WORKSPACE_DIR}/buildReleaseAsan

if [ ! -d ${BUILD_DIR} ]
then
	mkdir ${BUILD_DIR}
fi

cd ${BUILD_DIR}

if [ ! -f Makefile ]
then
	FLAGS="-fsanitize=address"
	cmake ${SOURCE_DIR} -DCMAKE_BUILD_TYPE=RelWithDebInfo -DENABLE_IPOPT=On -DIPOPT_INSTALL_PREFIX="/home/ilya/thesis/Ipopt/install/release/" -DCMAKE_CXX_FLAGS="${FLAGS}" -DCMAKE_C_FLAGS="${FLAGS}"
fi

if [ -f Makefile ]
then
	make -j4
else
	echo "No Makefile found!"
fi
