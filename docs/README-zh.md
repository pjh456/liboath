# liboath

`liboath` 是一个现代化的 C++ 错误处理与所有权管理工具库。

其灵感源自函数式语言和系统级语言对值语义的探索，结合 C++ 的强大类型系统，提供安全、简洁、可组合的编程抽象。

它旨在为 C++ 带来一套明确的、易于测试和扩展的值处理范式。

## 🚀 特性

- 💡 明确的值/引用/所有权语义划分
- 🧩 Rust 风格的 `Result<T, E>` 错误处理封装，支持链式调用
- 🔧 支持 `map` / `and_then` / `map_err` 等函数式变换
- 🔐 明确的所有权转移模型（如 `Owned<T>`）
- 🧪 易于测试、解耦良好的设计，适合大规模工程开发
- 🌱 零依赖，纯头文件实现

---

## 📦 模块结构

`liboath` 由若干核心模块组成，每个模块负责一种语义抽象：

| 模块名       | 功能描述                                      |
|--------------|-----------------------------------------------|
| `result/`     | `Result<T, E>` 错误处理模型，支持函数式调用链 |
| `value/`      | 值与引用语义的包装器，如 `Owned<T>` / `Ref<T>` 等 |
| `utils/`       | 辅助函数与模块集成                           |
| `test/`       | 单元测试与用例示例                           |

---

## 🛠️ 快速开始

```cpp
#include "result/result.hpp"
#include "value/owned.hpp"
#include <iostream>

using namespace oath;

Result<int, std::string> parse_int(const std::string& s) {
    try {
        return Result<int, std::string>::ok(std::stoi(s));
    } catch (...) {
        return Result<int, std::string>::err("Invalid number");
    }
}

int main() 
{
    auto result = parse_int("42")
        .map([](int x) { return x + 1; })
        .and_then([](int x)
        {
            if (x % 2 == 0) return Result<bool, std::string>::ok(true);
            return Result<bool, std::string>::err("odd");
        });

    if (result.is_ok()) std::cout << "Result: " << result.unwrap() << std::endl;
    else std::cerr << "Error: " << result.unwrap_err() << std::endl;
}
