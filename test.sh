#!/bin/bash
try() {
  expected="$1"
  input="$2"

  make > /dev/null
  ./comcom "$input" > tmp.s
  gcc -static -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" == "$expected" ]; then
    echo -e "$input \e[32m=> $actual\e[0m"
  else
    echo "$input: $expected expected, but got $actual"
    make clean
    exit 1
  fi
}

try 41 '12 + 34 - 5;'
try 47 "5+6*7;"
try 15 "5*(9-6);"
try 4 "(3+5)/2;"
try 2 "-4+6;"
try 0 '0<0;'
try 0 '1<0;'
try 1 '0<1;'
try 0 '0>0;'
try 0 '0>1;'
try 1 '1>0;'
try 1 '0<=0;'
try 0 '1<=0;'
try 1 '0<=1;'
try 1 '0>=0;'
try 0 '0>=1;'
try 1 '1>=0;'
try 1 '0==0;'
try 0 '0==1;'
try 0 '1!=1;'
try 1 '1!=0;'
try 6 'x = 3;x+3;'
echo -e "\e[33mAll Test Passed.\e[0m"
make clean
