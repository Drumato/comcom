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

try 41 'main(){return 12 + 34 - 5;}'
try 47 "main(){return 5+6*7;}"
try 15 "main(){return 5*(9-6);}"
try 4 "main(){return (3+5)/2;}"
try 2 "main(){return -4+6;}"
try 0 'main(){return 0<0;}'
try 0 'main(){return 1<0;}'
try 1 'main(){return 0<1;}'
try 0 'main(){return 0>0;}'
try 0 'main(){return 0>1;}'
try 1 'main(){return 1>0;}'
try 1 'main(){return 0<=0;}'
try 0 'main(){return 1<=0;}'
try 1 'main(){return 0<=1;}'
try 1 'main(){return 0>=0;}'
try 0 'main(){return 0>=1;}'
try 1 'main(){return 1>=0;}'
try 1 'main(){return 0==0;}'
try 0 'main(){return 0==1;}'
try 0 'main(){return 1!=1;}'
try 1 'main(){return 1!=0;}'
try 6 'main(){x = 3;return x+3;}'
try 6 'main(){foo = 3;bar = 3; return foo+bar;}'
try 6 'main(){x = 3;return x+3;}'
try 14 'main(){a = 3;b = 5 * 6 - 8;return a + b / 2;}'
try 30 'main(){a = 3;if(a) return 30;}'
try 50 'main(){a = 0;if(a) return 30; else return 50;}'
try 30 'main(){a = 3;if(a) return 30; else return 50;}'
try 0 'main(){a = 10;while(a) a = a-1; return a;}'
try 55 'main(){sum = 0;for(i = 1;i < 11;i = i + 1) sum = sum + i; return sum;}'
try 55 'main(){sum = 0;for(i = 1; i < 11;) {sum = sum + i;i = i+1;} return sum;}'
try 10 'foo(){return 10;} main(){ return foo();}'
try 33 'add(x){return x+3;} main(){ return add(30);}'
echo -e "\e[33mAll Test Passed.\e[0m"
make clean
