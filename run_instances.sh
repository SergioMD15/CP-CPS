#!/bin/bash

make

for f in small_instances/*
do
    printf "\n\n$f\n\n"
    timeout 1m ./main $f
    printf "\n\n"
done
