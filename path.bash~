#!/bin/bash

filepath="$(dirname "$(readlink -f "$0")")"

echo $filepath

echo "#define PATH "$filepath"" > ./C/Definitions/mpath.h
