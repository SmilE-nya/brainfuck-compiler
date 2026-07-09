#include <bits/stdc++.h>
int main(int c, char** v) {
    char *I = 0, *O = 0, e = 2;
    for (int a = 0; ++a < c;) {
        auto x = v[a];
        if (x[1] == 'v') puts("bfc v1"), exit(0);
        strstr(x, "-o") && ++a < c ? O = v[a],
                                     0
                                   : strstr(x, "em") && ++a < c
            ? e = *v[a] == 't'   ? 0
                  : *v[a] == 'c' ? 1
                  : *v[a] == 'e' ? 2
                                 : (exit(1), 0),
                                     0 : * x - '-' ? I = x, 0 : (exit(1), 0);
    }
    if (!I) exit(1);
    auto f = fopen(I, "r");
    if (!f) exit(1);
    auto s = new char[1 << 20]();
    int n = fread(s, 1, 1 << 20, f);
    if (!e) {
        for (int j = 0; j < n; ++j)
            if (strchr("><+-.,[]", s[j])) putchar(s[j]);
        exit(0);
    }
    int a[9], q = 0;
    auto g = fopen("z", "w");
    fputs("char t[999],*p=t;main(){", g);
    auto L = [&](int i) {
        int l = 1, c = 1, z = 0;
        for (; z < i;) s[z++] == 10 ? ++l, c = 1 : ++c;
        printf("%s:%d:%d ", I, l, c);
    };
    for (int j = 0; j < n; ++j)
        fputs(s[j] == 62   ? "++p;"
              : s[j] == 60 ? "--p;"
              : s[j] == 43 ? "++*p;"
              : s[j] == 45 ? "--*p;"
              : s[j] == 46 ? "putchar(*p);"
              : s[j] == 44 ? "*p=getchar();"
              : s[j] == 91 ? (a[q++] = j, "while(*p){")
              : s[j] == 93 ? (q > 0 ? --q : (L(j), puts("]"), q--), "}")
                           : "",
              g);
    for (int j = 0; j < q;) L(a[j++]), puts("[");
    fputs("}", g);
    fclose(g);
    if (q) exit(1);
    if (e & 1) return sprintf(s, "%s.c", O ? O : I), rename("z", s), puts(s), 0;
    sprintf(s, "gcc -x c -std=c89 -o %s z", O ? O : "a");
    return system(s) ? puts("x"), 1 : (puts("o"), 0);
}