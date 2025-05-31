# 更新日志 / Changelog

所有的变更记录都会在此列出。

## [0.0.1] - 2025-05-25
### Added
- 增加了对 ``Value<T>``, ``Result<T, E>`` 的实现和测试。

---

## [0.0.2] - 2025-05-30
### Added
- 添加了 README.md 项目简介与中英双语项目文档。

### Changed
- 修改了项目架构，移除了 src 文件夹。

## [0.0.3] - 2025-05-31
### Added
- 添加了用于配置环境的 config.h 宏头文件。
- 添加了对 ``Handle<T>``, ``Owned<T>``, ``Borrowed<T>`` 的实现。
- 添加了用于追踪所有权借用链的 borrow_stack_node.h 头文件。
- 添加了用于提供运行期友好报错信息的 runtime_check_utils.h 头文件。

### Changed
- 修改了 test_utils.h 头文件中用于测试函数的宏的写法，优化了运行期报错行为。

### Fixed
- 修复了中英双语项目文档中的成员方法名称问题。