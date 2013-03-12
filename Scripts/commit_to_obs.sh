#!/bin/bash

git_dir="${HOME}/git/"
obs_dir="${HOME}/obs/projects/home:ipalachev/polyhedron/"
name=polyhedron
version=2.0.1

cd ${git_dir}
cp -r ${name} ${name}-${version}
tar zcf ${name}-${version}.tar.gz ${name}-${version} --exclude=.git --exclude=poly-data-out
rm -rf ${name}-${version}
mv ${name}-${version}.tar.gz ${obs_dir}


