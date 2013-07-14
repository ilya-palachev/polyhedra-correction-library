#!/bin/bash

config=$1
cpp ${config} | grep "^[^#]" | sed ':a;N;$!ba;s/\n/;/g'
