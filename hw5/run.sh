#!/bin/bash

# clear
echo "Compile main.cpp ..."
g++ -o main 11055080_P5.cpp

echo -e "\n-------------- Running: whois.txt --------------"
./main ./testcase/whois.txt

echo -e "\n-------------- Running: zipf.txt --------------"
./main ./testcase/zipf.txt