#!/bin/bash

#
# greplace
#
# Globally replace one string with another in a set of files
#

SOURCE_DIRECTORIES="Source Tests External"
EXTENSIONS="*.h *.cpp *.c"

for dir in $SOURCE_DIRECTORIES
do
        for ext in $EXTENSIONS
        do
                find $dir -type f -name "$ext" -exec perl -p -i -e "s/$1/$2/g" '{}' ';'
        done
done
