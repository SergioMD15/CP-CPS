#!/bin/bash

make clean

make main

for f in instances/*
do
    printf "\n\n$f\n\n"
    timeout 2m ./main $f > output/$f
    printf "\n\n"
done
