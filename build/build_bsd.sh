#!/usr/local/bin/bash

#build project
cd ../freebsd
make -f EtherTerm.mk
cd ../build

# Quick Batch File to copy from Debug to current folder.
cp ../freebsd/Debug/EtherTerm .

echo 
echo 'Done!  ./EtherTerm to start Program'
