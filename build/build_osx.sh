#!/bin/bash

#build project
cd ../osx
make -f EtherTerm.mk
cd ../build

# Quick Batch File to copy from Debug to current folder.

cp ../osx/Debug/EtherTerm .

echo 
echo 'Done!  ./EtherTerm to start Program'
