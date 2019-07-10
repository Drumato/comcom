#!/bin/bash
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

try 41 '12 + 34 - 5'
echo -e "\e[32mOK\e[0m"
make clean
