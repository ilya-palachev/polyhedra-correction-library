#!/bin/bash

REPO_OLD="/home/iliya/polyhedron-old"
PREFIX_DIR="Polyhedron_join_facets_2\ -\ bu\ 20"
GIT_REPO="/home/iliya/workspace-git/polyhedron/"

pushd ${REPO_OLD}

ls | grep "${PREFIX_DIR}" | grep -v zip |
while read dir
do 
	export version=`echo ${dir} | sed -e "s/Polyhedron_join_facets_2\ -\ bu\ //g"`
	pushd ${GIT_REPO}
	git rm * -rf
	cp -rf ${REPO_OLD}/${dir}/* .
	git add .
	git commit -m "Version ${version} from old repository"
	git push
	popd
done

popd

exit 0
