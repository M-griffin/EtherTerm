#!/bin/bash

#build project
cd ../linux
make -f EtherTerm.mk
cd ../build

# Quick Batch File to copy from Debug to current folder.

cp ../linux/Debug/EtherTerm .

echo 
echo 'Done!  ./EtherTerm to start Program'
