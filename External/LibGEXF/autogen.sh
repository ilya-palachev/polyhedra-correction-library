#!/bin/bash

libtoolize --force && \
autoheader && \
aclocal && \
autoconf -f && \
automake --add-missing --gnu 
