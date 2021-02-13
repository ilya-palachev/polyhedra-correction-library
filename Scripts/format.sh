#!/bin/bash

clang-format -i $(find {Source,Tests,UnitTests} -regex ".*\.\(cpp\|c\|h\)")
