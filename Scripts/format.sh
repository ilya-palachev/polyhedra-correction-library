#!/bin/bash

clang-format-9 -i $(find {Source,Tests,UnitTests} -regex ".*\.\(cpp\|c\|h\)")
