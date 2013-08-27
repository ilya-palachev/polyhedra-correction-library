#!/bin/bash

sudo valgrind --trace-children=yes --db-attach=yes --track-origins=yes --leak-check=full $@
