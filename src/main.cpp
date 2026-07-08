#include<bits/stdc++.h>
int main(int c,char**v){
char*I=0,*O=0;int e=2;
for(int a=1;a<c;++a){
char*x=v[a];
if(!strcmp(x,"-v"))return puts("brainfuck-compiler v1.0.0"),0;
if(!strcmp(x,"-o")){if(++a<c)O=v[a];else goto u;continue;}
if(!strcmp(x,"--emit")&&++a<c){
e=*v[a]=='t'?0:*v[a]=='c'?1:*v[a]=='e'?2:(puts("bad"),-1);
if(e<0)return 1;continue;
}
if(*x=='-')return puts("?"),1;
I=x;
}
if(!I){
u:printf("Usage: %s <f> [-o o] [--emit t|c|e]\n",*v);return 1;
}
FILE*f=fopen(I,"r");
if(!f)return puts("no"),1;
fseek(f,0,SEEK_END);
int n=ftell(f);
rewind(f);
char*s=new char[n+1],*t=new char[n+1];
int*p=new int[n],k=0;
fread(s,1,n,f);
fclose(f);
s[n]=0;
for(int j=0;j<n;++j)
if(strchr("><+-.,[]",s[j]))t[k]=s[j],p[k]=j,++k;
t[k]=0;
if(!e)return puts(t),0;
int st[999],sp=0,err=0;
FILE*g=fopen("z","w");
fputs("#include <cstdio>\n#include <vector>\nint main(){\nstd::vector<unsigned char> t(1024,0);\nint p=0;\nauto e=[&](){if(p>=t.size())t.resize(t.size()*2,0);};\n",g);
auto L=[&](int i){
int l=1,c=1;
for(int z=0;z<i;++z)if(s[z]==10)++l,c=1;else++c;
printf("%s:%d:%d: error: ",I,l,c);
};
for(int i=0;i<k;++i){
switch(t[i]){
case '>':fputs("++p;e();\n",g);break;
case '<':fputs("--p;e();\n",g);break;
case '+':fputs("++t[p];\n",g);break;
case '-':fputs("--t[p];\n",g);break;
case '.':fputs("putchar(t[p]);\n",g);break;
case ',':fputs("t[p]=getchar();\n",g);break;
case '[':st[sp++]=i;fputs("while(t[p]){\n",g);break;
case ']':if(sp)--sp;else{L(p[i]);puts("unmatched ']'");err=1;}fputs("}\n",g);break;
}
}
for(int j=0;j<sp;++j){
L(p[st[j]]);puts("unmatched '['");err=1;
}
fputs("return 0;\n}\n",g);
fclose(g);
if(err)return remove("z"),1;
if(e==1){
char n[999];sprintf(n,"%s.cpp",O?O:I);
rename("z",n);printf("-> %s\n",n);return 0;
}
char x[999];
sprintf(x,"g++ -x c++ -O2 -o %s z",O?O:"a");
int r=system(x);
remove("z");
return r?puts("g++ fail"),1:(puts("Compilation succeeded"),0);
}
