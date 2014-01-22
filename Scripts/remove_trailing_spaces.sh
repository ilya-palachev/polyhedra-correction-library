#!/bin/bash

SOURCE_DIRECTORIES="Source Tests External"
EXTENSIONS="*.h *.cpp *.c"

for dir in $SOURCE_DIRECTORIES
do
	for ext in $EXTENSIONS
	do
		find $dir -type f -name "$ext" -exec sed -i 's/ *$//' '{}' ';'
	done
done
