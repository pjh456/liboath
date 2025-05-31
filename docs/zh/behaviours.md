# Oath 所有权建模库：行为说明

本文件记录 Oath 库中各类对象的生命周期、运行时行为与安全约定。

## ``Handle<T>``

- ``Handle<T>`` 作为 ``T*`` 的轻量级封装，只能跟随 ``Owned<T>`` 等所有权对象一起销毁，无析构函数。

- ``Handle<T>`` 非零成本抽象，内部存储了一个 ``BorrowStackNode*`` 用于追踪所有权借用栈堆路径。 ``BorrowStackNode`` 内部保存了借出所有权对象的 ``void*`` 指针和上一次借用的 ``BorrowStackNode*`` 指针。

- ``Handle<T>`` 可以通过 ``borrow_from(void*)`` 发生一次所有权借用，并自动构造一个新的 ``BorrowStackNode*`` 来记录本次借用。

- ``Handle<T>`` 可以通过 ``return_handle()`` 发生一次所有权归还，并自动析构保存的 ``BorrowStackNode*``，替换为上次借用的 ``BorrowStackNode*`` 指针。

- 若定义了 ``OATH_RUNTIME_CHECK`` 宏，则将使用 ``OATH_CHECK`` 宏在``return_handle()`` 时进行已发生借用检查，并抛出 ``std::runtime_error`` 异常。

## ``Owned<T>``

- ``Owned<T>`` 拥有对 ``T*`` 的唯一所有权，可以通过 ``borrow()`` 给出一个 ``Handle<T>*`` 表示所有权借出，借出后原对象内 ``Handle<T>*`` 变为空指针 ``nullptr``，此时不应对 ``Owned<T>`` 进行任何操作。

- ``Owned<T>`` 在构造时自动构造一个 ``Handle<T>*``，标明其所有权，并在析构时自动将其析构。

- 禁止拷贝和赋值，允许移动。

- 禁止手动构造，允许通过 ``Owned<T>::make_owned(T)`` 构造一个 ``Owned<T>``，允许手动和自动析构。

- 调用 ``.get()`` 或 ``operator->`` 获取 ``T*`` 时，默认假设内部指针不为空。

- 若定义了 ``OATH_RUNTIME_CHECK`` 宏，则将使用 ``OATH_CHECK`` 宏在调用 ``.get()`` 或 ``operator->`` 时进行空指针检查，并抛出 ``std::runtime_error`` 异常。

## ``Borrowed<T>``


