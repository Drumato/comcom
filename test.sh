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

try 41 'int main(){return 12 + 34 - 5;}'
try 47 "int main(){return 5+6*7;}"
try 15 "int main(){return 5*(9-6);}"
try 4 "int main(){return (3+5)/2;}"
try 2 "int main(){return -4+6;}"
try 0 'int main(){return 0<0;}'
try 0 'int main(){return 1<0;}'
try 1 'int main(){return 0<1;}'
try 0 'int main(){return 0>0;}'
try 0 'int main(){return 0>1;}'
try 1 'int main(){return 1>0;}'
try 1 'int main(){return 0<=0;}'
try 0 'int main(){return 1<=0;}'
try 1 'int main(){return 0<=1;}'
try 1 'int main(){return 0>=0;}'
try 0 'int main(){return 0>=1;}'
try 1 'int main(){return 1>=0;}'
try 1 'int main(){return 0==0;}'
try 0 'int main(){return 0==1;}'
try 0 'int main(){return 1!=1;}'
try 1 'int main(){return 1!=0;}'
try 6 'int main(){int x; x = 3;return x+3;}'
try 6 'int main(){int foo; int bar; foo = 3;bar = 3; return foo+bar;}'
try 6 'int main(){int x; x = 3;return x+3;}'
try 14 'int main(){int a; int b; a = 3;b = 5 * 6 - 8;return a + b / 2;}'
try 30 'int main(){int a; a = 3;if(a) return 30;}'
try 50 'int main(){int a; a = 0;if(a) return 30; else return 50;}'
try 30 'int main(){int a; a = 3;if(a) return 30; else return 50;}'
try 0 'int main(){ int a; a = 10;while(a) a = a-1; return a;}'
try 55 'int main(){ int sum; int i; sum = 0;for(i = 1;i < 11;i = i + 1) sum = sum + i; return sum;}'
try 55 'int main(){int i; int sum; sum = 0;for(i = 1; i < 11;) {sum = sum + i;i = i+1;} return sum;}'
try 10 'int foo(){return 10;} int main(){ return foo();}'
#try 33 'int add(int x){return x+3;} int main(){ return add(30);}'
#try 70 'int add(int x,int y){return x+y;} int main(){ return add(30,40);}'
try 3 'int main(){int x; int y; x = 3; y = &x; return *y;}'
try 3 'int main(){int x; int y; int z; x = 3; y = 5; z = &y + 8; return *z;}'
try 3 'int main(){int *x; *x = 3;return *x;}'
echo -e "\e[33mAll Test Passed.\e[0m"
make clean
