#!/bin/bash

make clean

make main

for f in ../instances/*
do
    filename=$(basename -- $f)
    timeout 1m ./main < $f > out/${filename%.*}.out
done
