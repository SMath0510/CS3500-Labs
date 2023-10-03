#!/bin/bash

g++ mlfq_naive.cpp
./a.out < incom/incom1.txt
./a.out < incom/incom2.txt
./a.out < incom/incom3.txt
./a.out < incom/incom4.txt
./a.out < incom/incom5.txt

 cat output/out1.txt output/out2.txt output/out3.txt output/out4.txt output/out5.txt
