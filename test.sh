#!/bin/bash
make > /dev/null
try() {
  expected="$1"
  input="$2"

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
try 6 'foo = 3;bar = 3; foo+bar;'
try 6 'x = 3;return x+3;'
try 14 'a = 3;b = 5 * 6 - 8;return a + b / 2;'
try 30 'a = 3;if(a) return 30;'
try 50 'a = 0;if(a) return 30; else return 50;'
try 30 'a = 3;if(a) return 30; else return 50;'
try 0 'a = 10;while(a) a = a-1; return a;'
try 55 'sum = 0;for(i = 1;i < 11;i = i + 1) sum = sum + i; return sum;'
try 55 'sum = 0;for(i = 1; i < 11;) {sum = sum + i;i = i+1;} return sum;'
echo -e "\e[33mAll Test Passed.\e[0m"
make clean
