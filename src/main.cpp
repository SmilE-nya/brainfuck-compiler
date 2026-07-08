#include<bits/stdc++.h>
int main(int c,char**v){
char*I=0,*O=0;int e=2;
for(int a=1;a<c;++a){
char*x=v[a];
if(!strcmp(x,"-v"))return puts("brainfuck-compiler v1.0.0"),0;
if(!strcmp(x,"-o")){if(++a<c)O=v[a];else goto u;continue;}
if(!strcmp(x,"--emit")&&++a<c){char m=*v[a];e=m=='t'?0:m=='c'?1:m=='e'?2:(puts("bad"),-1);if(e<0)return 1;continue;}
if(*x=='-')return puts("?"),1;
I=x;}
if(!I){u:printf("Usage: %s <f> [-o o] [--emit t|c|e]\n",*v);return 1;}
FILE*f=fopen(I,"r");if(!f)return puts("no"),1;
fseek(f,0,SEEK_END);int n=ftell(f);rewind(f);
char*s=(char*)malloc(n+1);fread(s,1,n,f);fclose(f);s[n]=0;
char*t=(char*)malloc(n+1);int*p=(int*)malloc(n*4),k=0;
for(int j=0;j<n;++j)if(strchr("><+-.,[]",s[j]))t[k]=s[j],p[k]=j,++k;t[k]=0;
if(e==0)return puts(t),0;
int st[9999],sp=0,err=0;
FILE*g=fopen("_t.cpp","w");
fputs("#include <iostream>\n#include <vector>\nint main(){\n"
"std::vector<unsigned char> tape(1024,0);\nint ptr=0;\n"
"auto ec=[&](){if(ptr>=(int)tape.size())tape.resize(tape.size()*2,0);};\n",g);
auto L=[&](int si){int l=1,c=1;for(int z=0;z<si&&z<n;++z)if(s[z]==10)++l,c=1;else++c;printf("%s:%d:%d: error: ",I,l,c);};
for(int i=0;i<k;++i){
switch(t[i]){
case'>':fputs("  ptr+=1;ec();\n",g);break;
case'<':fputs("  ptr-=1;ec();\n",g);break;
case'+':fputs("  tape[ptr]+=1;\n",g);break;
case'-':fputs("  tape[ptr]-=1;\n",g);break;
case'.':fputs("  std::cout<<tape[ptr];\n",g);break;
case',':fputs("  tape[ptr]=(unsigned char)std::cin.get();\n",g);break;
case'[':st[sp++]=i;fputs("  while(tape[ptr]){\n",g);break;
case']':
if(sp)--sp;else{L(p[i]);puts("unmatched ']'");err=1;}
fputs("  }\n",g);break;}}
for(int j=0;j<sp;++j){L(p[st[j]]);puts("unmatched '['");err=1;}
fputs("  return 0;\n}\n",g);fclose(g);
if(err)return remove("_t.cpp"),1;
if(e==1){char fn[999];sprintf(fn,"%s.cpp",O?O:I);rename("_t.cpp",fn);printf("-> %s\n",fn);return 0;}
char cmd[999];sprintf(cmd,"g++ -O2 -o %s _t.cpp",O?O:"a");
int r=system(cmd);remove("_t.cpp");
return r?puts("g++ fail"),1:(puts("Compilation succeeded"),0);}
