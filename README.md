# bfc — Brainfuck 编译器

将 Brainfuck 源码通过 C++ 中间代码，最终编译为本地可执行文件。

## 构建

```bash
# 需要: CMake 3.14+, g++ (支持 C++17)
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

构建产物为 `bfc`（或在 Windows 上为 `bfc.exe`）。

## 使用

```bash
bfc <input.bf> [-o <output>] [-O] [-v]
```

| 参数 | 说明 |
|------|------|
| `<input.bf>` | Brainfuck 源文件（必需） |
| `-o <output>` | 输出可执行文件名（默认 `a`） |
| `-O` | 启用 IR 优化（合并相邻 Move/Add 指令） |
| `-v` | 显示版本信息 |

### 示例

```bash
# 编译并运行 Hello World
bfc hello.bf -o hello
./hello            # → Hello World!

# 启用优化
bfc loop.bf -O -o loop_opt
./loop_opt

# 查看版本
bfc -v             # → bfc v1.0.0
```

## 架构

```
.bf 文件 → [Lexer] → 指令串 → [Parser] → 括号映射
                                              ↓
                   IR 列表 ← [IR Builder]
                        ↓
                   [Optimizer]（-O 时启用）
                        ↓
                   [Codegen] → C++ 源码 → g++ → 可执行文件
```

### 编译管线

1. **词法分析 (Lexer)** — 提取有效指令 `><+-.,[]`，忽略其他字符
2. **语法分析 (Parser)** — 检查括号匹配，构建括号位置映射
3. **IR 构建 (IR Builder)** — 转换为中间表示 (`IRNode` 列表)
4. **优化 (Optimizer)** — 合并相邻的 Move/Add 操作（仅 `-O` 时）
5. **代码生成 (Codegen)** — 生成 C++ 代码，使用 `std::vector` 动态纸带
6. **外部编译** — 调用 `g++ -O2` 将生成的 C++ 编译为可执行文件

## 已知限制

- **指针非负**: 纸带使用 `std::vector` 实现，指针 `ptr` 不会检查负值越界。确保 Brainfuck 程序不向负方向移动指针。
- **依赖 g++**: 需要系统 PATH 中存在 `g++`。

## 测试

```bash
cd build
bfc ../tests/hello.bf -o hello && ./hello
bfc ../tests/squares.bf -o squares && ./squares
bfc ../tests/error_unmatched.bf     # 应报告括号不匹配
```

测试文件位于 `tests/` 目录下。
