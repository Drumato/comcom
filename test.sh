#!/bin/bash
rake > /dev/null
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
    rake clean
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
try 6 'int main(){int x = 3;return x+3;}'
try 6 'int main(){int foo = 3; int bar =3; return foo+bar;}'
try 6 'int main(){int x = 3;return x+3;}'
try 14 'int main(){int a = 3; int b = 5 * 6 - 8;return a + b / 2;}'
try 30 'int main(){int a = 3;if(a) return 30;}'
try 50 'int main(){int a = 0;if(a) return 30; else return 50;}'
try 30 'int main(){int a = 3;if(a) return 30; else return 50;}'
try 0 'int main(){ int a = 10;while(a) a = a-1; return a;}'
try 55 'int main(){ int sum = 0; int i; for(i = 1;i < 11;i = i + 1) sum = sum + i; return sum;}'
try 55 'int main(){int i; int sum = 0;for(i = 1; i < 11;) {sum = sum + i;i = i+1;} return sum;}'
try 10 'int foo(){return 10;} int main(){ return foo();}'
try 33 'int add(int x){return x+3;} int main(){ return add(30);}'
try 70 'int add(int x,int y){return x+y;} int main(){ return add(30,40);}'
try 3 'int main(){int a = 3; int *x = &a; return *x;}'
try 33 'int add(int *x){return *x+3;} int main(){ int a = 30;return add(&a);}'
try 8 'int main(){ int a; return sizeof(a);}'
try 8 'int main(){ int *x; return sizeof(*x);}'
try 8 'int main(){ int *x; return sizeof(x);}'
try 8 'int main(){ int *x; return sizeof(sizeof(x));}'
try 8 'int main(){ return sizeof("Drumato");}'
try 18 'int main(){ struct{ int val; int *ptr; char c; char b;} x; return sizeof(x);}'
try 80 'int main(){ int a[10]; return sizeof(a);}'
try 1 'int main(){int a[2]; *a = 1; *(a + 1) = 2; return *a;}'
try 2 'int main(){int a[2]; *a = 1; *(a + 1) = 2; return *(a+1);}'
try 3 'int main(){int a[2]; *a = 1; *(a + 1) = 2; return *a+*(a+1);}'
try 1 'int main(){int a[2]; a[0] = 1; a[1] = 2; return a[0];}'
try 2 'int main(){int a[2]; a[0] = 1; a[1] = 2; return a[1];}'
try 3 'int main(){int a[2]; a[0] = 1; a[1] = 2; return a[0]+a[1];}'
try 30 'int a; int main(){ a = 30; return a;}'
try 60 'int a[2]; int main(){ a[1] = 60; return a[1];}'
try 30 'int main(){ struct{int val;}x; x.val = 30; return x.val;} '
try 30 'int main(){ struct{int val;int v; }x; x.v = 30; return x.v;} '
try 60 'int main(){ struct{int val;int v; }x;x.val = 30;  x.v = 30; return x.val + x.v;} '
echo -e "\e[33mAll Test Passed.\e[0m"

rake clean
