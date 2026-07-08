#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// BF 指令集合
static bool is_bf(char c) {
    return c == '<' || c == '>' || c == '+' || c == '-' || c == '.' ||
           c == ',' || c == '[' || c == ']';
}

int main(int argc, char* argv[]) {
    std::string in, out;
    int emit = 2;  // 0=tokens, 1=cpp, 2=exe

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "-v") {
            std::cout << "brainfuck-compiler v1.0.0\n";
            return 0;
        } else if (a == "-o") {
            if (++i < argc)
                out = argv[i];
            else
                goto u;
        } else if (a == "--emit" && ++i < argc) {
            char m = *argv[i];
            emit = m == 't'   ? 0
                   : m == 'c' ? 1
                   : m == 'e' ? 2
                              : (std::cerr << "bad\n", -1);
            if (emit < 0) return 1;
        } else if (a[0] == '-') {
            std::cerr << "?\n";
            return 1;
        } else
            in = a;
    }
    if (in.empty()) {
    u:
        std::cerr << "Usage: " << argv[0] << " <f> [-o o] [--emit t|c|e]\n";
        return 1;
    }

    // 读源文件
    std::ifstream f(in);
    if (!f) {
        std::cerr << "no " << in << "\n";
        return 1;
    }
    std::string src((std::istreambuf_iterator<char>(f)), {});

    // 词法分析：提取有效指令 + 记录每个 token 在源文件中的位置
    std::string tok;
    std::vector<int> pos;  // token[i] 对应 src 中的下标
    for (int i = 0; i < (int)src.size(); ++i) {
        if (is_bf(src[i])) {
            tok += src[i];
            pos.push_back(i);
        }
    }

    if (emit == 0) {
        std::cout << tok;
        return 0;
    }

    // 将源文件下标转为 (行,列)
    auto loc = [&](int si, int& l, int& c) {
        l = 1;
        c = 1;
        for (int j = 0; j < si && j < (int)src.size(); ++j)
            if (src[j] == '\n') {
                ++l;
                c = 1;
            } else
                ++c;
    };

    // 收集全局错误，排序后输出（按行、列顺序）
    struct Err {
        int l, c;
        std::string msg;
    };
    std::vector<Err> errs;

    // 括号匹配 + C++ 代码生成（一次遍历）
    std::vector<int> st;  // 存 '[' 在 tok 中的下标
    std::string cpp =
        "#include <iostream>\n#include <vector>\nint main(){\n"
        "std::vector<unsigned char> tape(1024,0);\nint ptr=0;\n"
        "auto "
        "ec=[&](){if(ptr>=(int)tape.size())tape.resize(tape.size()*2,0);};\n";

    for (int i = 0; i < (int)tok.size(); ++i) {
        switch (tok[i]) {
            case '>':
                cpp += "  ptr+=1;ec();\n";
                break;
            case '<':
                cpp += "  ptr-=1;ec();\n";
                break;
            case '+':
                cpp += "  tape[ptr]+=1;\n";
                break;
            case '-':
                cpp += "  tape[ptr]-=1;\n";
                break;
            case '.':
                cpp += "  std::cout<<tape[ptr];\n";
                break;
            case ',':
                cpp += "  tape[ptr]=(unsigned char)std::cin.get();\n";
                break;
            case '[':
                st.push_back(i);
                cpp += "  while(tape[ptr]){\n";
                break;
            case ']': {
                int l, c;
                loc(pos[i], l, c);
                if (st.empty())
                    errs.push_back({l, c, "unmatched ']'"});
                else
                    st.pop_back();
                cpp += "  }\n";
            } break;
        }
    }
    for (int s : st) {
        int l, c;
        loc(pos[s], l, c);
        errs.push_back({l, c, "unmatched '['"});
    }

    if (!errs.empty()) {
        std::sort(errs.begin(), errs.end(), [](auto& a, auto& b) {
            return a.l < b.l || (a.l == b.l && a.c < b.c);
        });
        for (auto& e : errs)
            std::cerr << in << ":" << e.l << ":" << e.c << ": error: " << e.msg
                      << "\n";
        return 1;
    }
    cpp += "  return 0;\n}\n";

    if (emit == 1) {
        std::string fn = out.empty() ? in + ".cpp" : out + ".cpp";
        std::ofstream o(fn);
        if (!o) {
            std::cerr << "no " << fn << "\n";
            return 1;
        }
        o << cpp;
        std::cout << "-> " << fn << "\n";
        return 0;
    }

    std::ofstream o("_t.cpp");
    o << cpp;
    o.close();
    int r = std::system(
        ("g++ -O2 -o " + (out.empty() ? "a" : out) + " _t.cpp").c_str());
    std::remove("_t.cpp");
    if (r) {
        std::cerr << "compilation error (see above)\n";
        return 1;
    }
    std::cout << "Compilation succeeded\n";
    return 0;
}