# 工作计划：Brainfuck 编译器 (bfc)

## 项目概述
将 Brainfuck 源码翻译为 C++ 代码，再通过 g++ 编译为可执行文件。
使用 `std::vector` 动态扩展纸带，指针默认非负。

## 系统环境
- OS: Windows (MSYS2)
- 编译器: g++ 16.1.0
- CMake: 4.3.1
- 构建: CMake (C++17)
- 测试: .bf 测试文件 + 手动验证

## CLI 接口
```
bfc <input.bf> [-o <output>] [-O] [-v]
```
- `<input.bf>`：源文件（必需）
- `-o <output>`：输出文件名（默认 `a`）
- `-O`：启用 IR 优化
- `-v`：显示版本信息

## 架构流程
```
.bf 文件 → [Lexer] → 指令串 → [Parser] → 括号映射
                                               ↓
                    IR 列表 ← [IR Builder] ← 指令串 + 映射
                         ↓
                    [Optimizer]（仅 -O 时）
                         ↓
                    [Codegen] → C++ 源码 → g++ → 可执行文件
```

## 各阶段详细设计

### Phase 1: 项目骨架
- 创建目录结构：`src/`, `tests/`
- 编写 `CMakeLists.txt`（C++17，可执行文件 `bfc`）
- 将 `main.cpp` 移入 `src/`
- 配置 .gitignore（忽略临时文件 `_temp_bf_output.cpp`、构建目录）

### Phase 2: 词法分析器 (Lexer)
- **输入**: `const std::string& source_code`
- **输出**: `std::string`（仅包含 `><+-.,[]` 的有效字符）
- 遍历源文件，过滤出有效 Brainfuck 指令
- 错误处理：若文件为空字符串，返回空串（非错误）

### Phase 3: 语法分析器 (Parser)
- **输入**: `const std::string& tokens`
- **输出**: `bool` + `std::unordered_map<int, int>& bracket_map`
- 使用 `std::stack<int>` 匹配 `[` 和 `]`
- 映射同时包含正向（`[` → `]`）和反向（`]` → `[`）
- 失败时打印位置（字符索引）并返回 false
- 额外的验证：不允许嵌套深度过大（暂不限制，但注意栈溢出）

### Phase 4: 中间表示 (IR)
- **IRKind 枚举**: `Move`, `Add`, `Put`, `Get`, `LoopStart`, `LoopEnd`
- **IRNode 结构**: `kind`, `value`（偏移量）, `target`（循环配对索引）
- **buildIR**: 遍历 tokens，根据 bracket_map 填充 target
  - `>` → Move(+1)，`<` → Move(-1)
  - `+` → Add(+1)，`-` → Add(-1)
  - `.` → Put，`,` → Get
  - `[` → LoopStart，`]` → LoopEnd

### Phase 5: 优化器 (Optimizer)
- 合并相邻的 Move 节点和 Add 节点
- 算法：扫描 IR，累积 deltaMove 和 deltaAdd，遇到非 Move/Add 时 flush
- 重新映射 LoopStart/LoopEnd 的 target 索引：
  1. 先清除所有 target = -1
  2. 用栈重新遍历 IR 配对
- **仅在用户指定 -O 时调用**

### Phase 6: 代码生成器 (Codegen)
- **C++ 代码模板**:
  - 头文件: `<iostream>`, `<vector>`, `<cstring>`, `<fstream>`
  - `std::vector<unsigned char> tape(1024, 0)` 初始纸带
  - `int ptr = 0` 指针
  - `ensure_capacity` lambda：越界时翻倍扩容
- **翻译规则**:
  - Move(delta) → `ptr += delta; ensure_capacity();`
  - Add(delta) → `tape[ptr] += delta;`
  - Put → `std::cout << tape[ptr];`
  - Get → `tape[ptr] = std::cin.get();`
  - LoopStart → `while (tape[ptr]) {`
  - LoopEnd → `}`
- **外部编译**: 写入 `_temp_bf_output.cpp`，调用 `g++ -O2 -o <output> _temp_bf_output.cpp`
- 编译成功后删除临时文件
- Windows 上输出文件自动添加 `.exe`（在 main.cpp 中处理）

### Phase 7: 主程序集成
- 解析命令行参数（argv 遍历）
- 读取源文件（ifstream）
- 按流程调用各阶段
- 错误处理：文件找不到、括号不匹配、编译失败均输出友好错误信息
- `-v` 显示版本号 `bfc v1.0.0`

### Phase 8: 测试用例
| 文件 | 内容 | 测试目标 |
|------|------|---------|
| `tests/hello.bf` | Hello World 程序 | 基本功能 |
| `tests/loop.bf` | 嵌套循环 | 循环处理 |
| `tests/error.bf` | 括号不匹配 | 错误报告 |
| `tests/move.bf` | 大量 `>` 移动 | 动态扩容 |
| `tests/opt.bf` | 连续 `+++>--<.` | 优化效果 |

### Phase 9: README + 收尾
- 更新 README.md：构建方法、使用示例、参数说明、已知限制

## 依赖关系与并行策略
```
Phase 1 (CMake + 目录) — 独立先行
      │
      ├── Phase 2 (Lexer) — 独立
      │        │
      │        └── Phase 3 (Parser) — 依赖 Phase 2 的接口签名
      │                 │
      │                 └── Phase 4 (IR Builder) — 依赖 Phase 3
      │                          │
      │                          ├── Phase 5 (Optimizer) — 依赖 Phase 4
      │                          │
      │                          └── Phase 6 (Codegen) — 依赖 Phase 4
      │                                   │
      └───────────────────────────────────┴── Phase 7 (Main) — 依赖 Phase 2-6
                                                   │
                                                   └── Phase 8 (Tests)
                                                           │
                                                           └── Phase 9 (README)
```

## 验收标准
- [ ] `bfc tests/hello.bf -o hello` → 生成 `hello.exe`，运行打印 "Hello World!"
- [ ] `bfc tests/loop.bf -O -o loop` → 优化后编译，运行输出正确
- [ ] `bfc tests/error.bf` → 输出括号不匹配错误信息并退出
- [ ] `bfc tests/move.bf -o move` → 纸带自动扩容，运行正确
- [ ] `bfc -v` → 显示版本号
- [ ] `bfc`（无参数）→ 显示用法信息
- [ ] `bfc nonexistent.bf` → 报告文件不存在
