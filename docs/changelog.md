# 更新日志 / Changelog

所有的变更记录都会在此列出。

## [0.0.1] - 2025-05-25
### Added
- 增加了对 ``Value<T>``, ``Result<T, E>`` 的实现和测试。

---

## [0.0.2] - 2025-05-30
### Added
- 添加了 [README.md](../README.md) 项目简介与中英双语项目文档。

### Changed
- 修改了项目架构，移除了 src 文件夹。

---

## [0.0.3] - 2025-05-31
### Added
- 添加了用于配置环境的 [config.h](../include/oath20/config.h) 宏头文件。
- 添加了对 ``Handle<T>``, ``Owned<T>``, ``Borrowed<T>`` 的实现。
- 添加了用于追踪所有权借用链的 [borrow_stack_node.h](../include/oath20/ownership/borrow_stack_node.h) 头文件。
- 添加了用于提供运行期友好报错信息的 [runtime_check_utils.h](../include/oath20/utils/runtime_check_utils.h) 头文件。

### Changed
- 修改了 [test_utils.h](../include/oath20/utils/test_utils.h) 头文件中用于测试函数的宏的写法，优化了运行期报错行为。

### Fixed
- 修复了中英双语项目文档中的成员方法名称问题。

---

## [0.0.4] - 2025-05-31
### Added
- 添加了类型行为解释文档 [behaviours.md](zh/behaviours)。
- 为更新日志添加了文件索引。

### Changed
- 修改了项目文档路径，区分开了不同语言。
- 修改了 ``Owned<T>`` 的构造权限，现在参数为 ``Handle<T>*`` 的构造函数权限为 private。

### Fixed
- 修复了更新日志中格式不统一的问题。

---

## [0.0.5] - 2025-05-31
### Added
- 添加了设计思路文档 [design_note.md](zh/design_note.md)。
- 添加了类型行为解释文档 [behaviours.md](zh/behaviours) 中关于 ``Borrowed<T>`` 的部分。

### Changed
- 修改了 [README.md](zh/README.md) 里的命名规范速览模块，使其更简洁易懂。
- 修改了类型行为解释文档 [behaviours.md](zh/behaviours) 中关于 ``Owned<T>`` 的部分表述。

---
