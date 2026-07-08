#include<bits/stdc++.h>
int main(int c,char**v){
char*I=0,*O=0;int e=2;
for(int a=1;a<c;++a){
char*x=v[a];
if(!strcmp(x,"-v"))return puts("bfc v1"),0;
if(!strcmp(x,"-o")){if(++a>=c)exit(1);O=v[a];continue;}
if(!strcmp(x,"--emit")&&++a<c){
e=*v[a]=='t'?0:*v[a]=='c'?1:*v[a]=='e'?2:(exit(1),0);continue;
}
if(*x=='-')exit(1);
I=x;
}
if(!I)printf("Usage:%s f [-o o] [--emit t|c|e]\n",*v),exit(1);
auto f=fopen(I,"r");
if(!f)exit(1);
fseek(f,0,2);
int n=ftell(f);
rewind(f);
auto s=new char[n+1](),t=new char[n+1]();
int*p=new int[n],k=0;
fread(s,1,n,f);
fclose(f);
for(int j=0;j<n;++j)
if(strchr("><+-.,[]",s[j]))t[k]=s[j],p[k]=j,++k;
if(!e)return puts(t),0;
int a[99],q=0,z=0;
auto g=fopen("z","w");
fputs("#include<bits/stdc++.h>\nint main(){static unsigned char t[1<<20],*p=t;",g);
auto L=[&](int i){
int l=1,c=1,z=0;for(;z<i;)if(s[z++]==10)++l,c=1;else++c;
printf("%s:%d:%d ",I,l,c);
};
for(int i=0;i<k;++i){
switch(t[i]){
case '>':fputs("++p;",g);break;
case '<':fputs("--p;",g);break;
case '+':fputs("++*p;",g);break;
case '-':fputs("--*p;",g);break;
case '.':fputs("putchar(*p);",g);break;
case ',':fputs("*p=getchar();",g);break;
case '[':a[q++]=i;fputs("while(*p){",g);break;
case ']':q?--q:(L(p[i]),puts("]"),z=1);fputs("}",g);break;
}
}
for(int j=0;j<q;)L(p[a[j++]]),puts("["),z=1;
fputs("}",g);
fclose(g);
if(z)exit(1);
if(e==1){
char n[99];sprintf(n,"%s.cpp",O?O:I);
rename("z",n);printf("-> %s\n",n);return 0;
}
char x[99];
sprintf(x,"g++ -x c++ -o %s z",O?O:"a");
return system(x)?puts("x"),1:(puts("o"),0);
}
