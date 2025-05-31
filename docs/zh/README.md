# Oath: 为现代 C++ 设计的更安全所有权建模

## 项目简介

**Oath** 是一个轻量级、低耦合、低成本抽象的 C++ 所有权建模库，旨在为 C++ 提供接近 Rust 的资源管理体验，同时保持对原生 STL 代码和第三方代码的良好兼容性。

Oath 不引入 GC，不重建语法，而是在不干扰开发者开发习惯的基础上，将所有权、安全性和表达力有机结合。

该项目以 C++20 为主干实现，并将在未来针对 C++14 与 C++17 提供版本支持，使用现代语义封装原生资源管理模式，为大型工程和历史代码重构提供一套渐进式、非侵入式、高性能的解决方案。

## 核心理念

- **显式所有权**：用 ``Owned<T>``, ``Borrowed<T>``, ``Value<T>`` 等类型标明变量的资源语义，消除裸指针行为不确定性。

- **可视化转移链**：通过 ``Handle<T>`` 作为轻量裸指针包装，串联资源的持有者和使用者，控制访问流向。

- **低开销封装**：类型实现不引入虚表，不使用 RTTI，不阻碍编译期优化路径，仅在资源所有权和生命周期检查上引入运行时检查。（可在 Release 版本中启用宏来优化）。

- **语义化类型**：通过区分拷贝值 ``Value<T>``、借用所有权 ``Borrowed<T>`` 、单一所有权 ``Owned<T>`` 和只读类型 ``Readonly<T>``，将函数接口传参的逻辑显式暴露，提高函数接口的自解释性。

- **可控异常处理**：引入 ``Result<T, E>``, ``Option<T>`` 类型作为返回值，将函数分支显式处理，作为 noexcept 的保证来提供编译器优化。

## 类型体系说明

### ``Handle<T>``

表示**资源的所有权**，即管理资源生命周期的负责者，存在于各类所有权类型之中。

- 无析构逻辑，必须通过 ``Owned<T>`` 等所有权类型自动管理；

- 可通过类型接口转移 ``Handle<T>`` 的归属，具象化转移过程并自动构造新类型；

- 拥有调用链逻辑，可以追踪借用连上所有变量，并逐步归还所有权。

- 可被截获用于低层次内存访问（信任开发者使用）。

### ``Owned<T>``

表示**唯一拥有该资源所有权**的变量：

- 负责资源析构，析构时自动释放资源；

- 不可拷贝，可移动；

- 可通过 ``.borrow()`` 将自身所有权暂时借出，直到归还前都不允许再次使用。

### ``Borrowed<T>``

表示**唯一暂时借用所有权**的变量：

- 不负责资源析构，无析构逻辑；

- 不可拷贝，不可移动，生命周期不可脱离原持有者；

- 可通过 ``.borrow()`` 将借来的所有权暂时借出，直到归还前都不允许再次使用；

- 可通过 ``.return()`` 归还所有权给上一任借用者。

### ``Value<T>``

表示一个**显式拷贝值**的变量：

- 拥有值语义，但不参与所有权管理；

- 可拷贝，不可移动，在构造时自动拷贝 ``Handle<T>`` 存在的资源；

## 项目结构与构建支持

```
oath/
├── docs/
│   ├── architecture.md     # 项目架构
│   └── changelog.md        # 版本日志
├── include/
│   ├── oath20/             # C++20 实现（主干，目前命名为 oath）
│   ├── oath17/             # C++17 实现（暂未支持）
│   ├── oath14/             # C++14 实现（暂未支持）
│   └── oath.hpp            # 自动版本适配头文件
├── test/                   # 单元测试
├── examples/               # 示例程序
└── CMakeLists.txt          # 多版本构建脚本（暂未支持）
```

## 用法示例

```cpp
#include <string>
#include <iostream>

#include "oath.hpp"

using namespace std;
using namespace oath;

class File
{
private:
    char* data;
    size_t length;

public:
    File(const char* file_path);
    File(const File& other); // Make the Value<File> available.
    ~File();

public:
    static File* open(const char* file_path);
    bool is_open() const; // Note: only const functions and fields can be called in ReadOnly<File>, which also can be seemed as "view" just like string_view.
    void copy_self() noexcept; // It will change data inside itself, so the ownership(handle) is needed to be given.
};

Result<bool, string> checkFileAvailable(Readonly<File> file)
{
    try
    {
        return Result<bool, string>::ok(file.is_open());
    }
    catch(...)
    {
        return Result<bool, string>::err("File open error!");
    }
}

void handleFileData(Borrowed<File> file)
{
    file.copy_self();
    file.return_handle(); // return the handle, or the handle won't go back to Owned<File>.
}

void saveFile(Value<File> file)
{
    /* I have no idea how to give a example of it,
    but actually it is a copy of the original file. */
}

int main()
{
    Owned<File> file = make_owned(File::open("example.txt"));

    auto result = checkFileAvailable(file); // get a result to check if the function is going wrong.
    auto ok_match_func = [](bool val) { return val; };
    auto err_match_func = [](string&& err) { return false; };
    auto match_val = result.match(ok_match_func, err_match_func); // match the value of result repeatedly. make sure their returned type of value are the same.
    cout << match_val ? "available" : "unavailable" << endl;

    handleFileData(file.borrow()); // borrow the handle so that the data inside it can be change but can't be destoried.

    saveFile(file); // the only thing you have to do is to give a Owned<File>, and it will copy itself if it can.
}
```

## 项目优势

- 性能：零抽象成本，紧贴裸指针语义；支持编译期优化

- 安全性：显式所有权，限制悬垂、重删、资源泄漏等典型问题

- 可集成性：与 STL、原始指针、现有代码兼容，适配成本低

- 学习曲线：语义明确，无需宏魔法，符合现代 C++ 用户习惯

- 向后兼容：支持 C++14~20，跨项目迁移成本低

## 使用场景

- 安全替代 unique_ptr, shared_ptr 等在语义上不严谨的使用场景；

- 对外暴露接口资源时明确转移边界（如库函数/回调传参）；

- 模块内组件隔离，确保指针传递无越界访问；

- 历史工程的资源管理重构，渐进式修复；

## 后续计划

- ⏳ 完成最小版本实现

- ⏳ 完善对各主流标准的支持：C++14/17/20

- ⏳ 提供与 STL 接口兼容的类型适配器

- ⏳ 引入 shared 与 weak 子模块（在安全基础上）

- ⏳ 增加编译期静态检查机制（诊断未迁移裸资源）

- ⏳ 完成标准库中常用容器的所有权包装器适配

- ⏳ 完善与 Boost、Abseil 的互操作性测试

## 许可证与贡献

Oath 使用 MIT 协议，鼓励企业与个人在重构、审计、现代化迁移中采用。 欢迎在 GitHub 提交 Issues、PRs 与建议。

## 联系我们

有事 Issues 见。

## 附：命名规范速览

|语义|类型名|描述|
|--|--|--|
|裸指针封装|``Handle<T>``|可由``Owned<T>``等导出，构造``Borrowed<T>``等类型|
|拥有资源|``Owned<T>``|拥有生命周期，负责释放|
|借用资源|``Borrowed<T>``|引用持有者的资源，不参与析构，需手动归还所有权|
|值拷贝|``Value<T>``|不拥有资源，仅拷贝值|
|独占资源|``Unique<T>``|独占生命周期，自动释放|
|共享资源|``Shared<T>``|单可写+多只读共享，引用计数为 0 时自动释放|
|观察引用|``Weak<T>``|不参与引用计数|

> Oath，不是要替代语言，也不是教训开发者——而是用最少的门槛，给现代 C++ 一个看得见、摸得着的内存安全未来。

