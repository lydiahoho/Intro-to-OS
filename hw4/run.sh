#!/bin/bash


gcc -o main main.c
gcc -fPIC -g -shared -o bf.so bf.c

gcc -fPIC -g -shared -o ff.so ff.c

echo -e "Best Fit:"
LD_PRELOAD=./bf.so ./main

echo -e "----------------------------\nFirst Fit:"
LD_PRELOAD=./ff.so ./main
