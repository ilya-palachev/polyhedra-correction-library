#!/bin/bash

astyle -T8 --max-code-length=80 --break-after-logical $@
