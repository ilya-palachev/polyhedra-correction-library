#!/bin/bash

/home/ilya/git/opensource/gcc/build/gcc/xg++ -B/home/ilya/git/opensource/gcc/build/gcc -I/home/ilya/git/opensource/gcc/build/x86_64-unknown-linux-gnu/libstdc++-v3/include -I/home/ilya/git/opensource/gcc/build/x86_64-unknown-linux-gnu/libstdc++-v3/include/x86_64-unknown-linux-gnu -I/home/ilya/git/opensource/gcc/libstdc++-v3/libsupc++ -Wno-deprecated-declarations -L/home/ilya/git/opensource/gcc/build/x86_64-unknown-linux-gnu/libstdc++-v3/src/.libs/ $@
