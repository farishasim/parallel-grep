#!/bin/bash

cd $1
./compile.sh
cd ..
mv $1/grep .