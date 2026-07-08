#include<bits/stdc++.h>
int main(int c,char**v){
char*I=0,*O=0;int e=2;
for(int a=1;a<c;++a){
auto x=v[a];
if(x[1]=='v')puts("bfc v1"),exit(0);
if(!strcmp(x,"-o")&&++a<c)O=v[a];else if(strstr(x,"emit")&&++a<c)e=*v[a]=='t'?0:*v[a]=='c'?1:*v[a]=='e'?2:(exit(1),0);else if(*x-'-')I=x;else exit(1);
}
if(!I)printf("U:%s f[-o o][-e t|c|e]\n",*v),exit(1);
auto f=fopen(I,"r");
if(!f)exit(1);
fseek(f,0,2);
int n=ftell(f);
rewind(f);
auto s=new char[n+1],t=new char[n+1]();
int*p=new int[n],k=0;
fread(s,1,n,f);
for(int j=0;j<n;++j)
if(strchr("><+-.,[]",s[j]))t[k]=s[j],p[k]=j,++k;
if(!e)puts(t),exit(0);
int a[99],q=0;
auto g=fopen("z","w");
fputs("#include<cstdio>\nchar t[1<<20],*p=t;int main(){",g);
auto L=[&](int i){
int l=1,c=1,z=0;for(;z<i;)if(s[z++]==10)++l,c=1;else++c;
printf("%s:%d:%d ",I,l,c);
};
for(int i=0;i<k;++i)switch(t[i]){
case '>':fputs("++p;",g);break;
case '<':fputs("--p;",g);break;
case '+':fputs("++*p;",g);break;
case '-':fputs("--*p;",g);break;
case '.':fputs("putchar(*p);",g);break;
case ',':fputs("*p=getchar();",g);break;
case '[':a[q++]=i;fputs("while(*p){",g);break;
case ']':q>0?--q:(L(p[i]),puts("]"),q--);fputs("}",g);break;
}
for(int j=0;j<q;)L(p[a[j++]]),puts("[");
fputs("}",g);
fclose(g);
if(q)exit(1);
char x[99];
if(e&1)return sprintf(x,"%s.c",O?O:I),rename("z",x),puts(x),0;
sprintf(x,"g++ -x c++ -o %s z",O?O:"a");
return system(x)?puts("x"),1:(puts("o"),0);
}
