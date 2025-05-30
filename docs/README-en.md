# Oath: A Safer Ownership Model for Modern C++

## Project Overview

**Oath** is a lightweight, low-coupling, low-cost abstraction C++ ownership modeling library, aiming to bring Rust-like resource management to C++ while maintaining good compatibility with native STL code and third-party libraries.

Oath does not introduce a garbage collector or redesign syntax. Instead, it organically combines ownership, safety, and expressiveness without disrupting developer habits.

The project is implemented primarily in C++20 and will support C++14 and C++17 in the future. It encapsulates native resource management with modern semantics, offering a progressive, non-intrusive, high-performance solution for large-scale engineering and legacy code refactoring.

## Core Philosophy

- **Explicit Ownership**: Use types like ``Owned<T>``, ``Borrowed<T>``, ``Value<T>`` to denote resource semantics of variables, eliminating the uncertainty of raw pointers.

- **Visual Transfer Chain**: Use ``Handle<T>`` as a lightweight wrapper of raw pointers, connecting resource owners and users, and controlling access flow.

- **Low-Overhead Abstraction**: Types do not use virtual tables, avoid RTTI, and do not block compile-time optimizations. Runtime checks are only introduced for ownership and lifetime management. (Macros can be enabled for optimization in Release builds.)

- **Semantic Types**: By distinguishing copy value ``Value<T>``, borrowed ownership ``Borrowed<T>``, unique ownership ``Owned<T>``, and readonly type ``Readonly<T>``, function interface semantics are made explicit, improving self-descriptiveness.

- **Controllable Error Handling**: Introduce types like ``Result<T, E>`` and ``Option<T>`` as return values, making function branches explicit and enabling compiler optimization as a guarantee for ``noexcept``.

## Type System Explanation

### ``Handle<T>``

Represents **ownership of a resource**, i.e., the entity responsible for managing its lifecycle, and exists within all ownership types.

- Has no destructor logic; must be managed automatically via ``Owned<T>`` or other ownership types.

- Transfer of ownership can be done through type interfaces, making transfer explicit and constructing new types automatically.

- Supports invocation chain logic, allowing tracking of all variables borrowing the resource and progressively returning ownership.

- Can be intercepted for low-level memory access (trusted to the developer).

### ``Owned<T>``

Represents a variable that **exclusively owns the resource**:

- Responsible for destructing the resource and releasing it automatically.

- Non-copyable, movable.

- Can temporarily lend its ownership via ``.borrow()``, during which reuse is forbidden until returned.

### ``Borrowed<T>``

Represents a variable that **temporarily borrows ownership**:

Not responsible for resource destruction; no destructor logic.

Non-copyable, non-movable, and its lifetime cannot outlive the original owner.

Can temporarily lend the borrowed ownership via ``.borrow()``, during which reuse is forbidden until returned.

Ownership can be returned to the previous borrower via ``.return()``.

### ``Value<T>``

Represents a variable that is an **explicit copy of value**:

Has value semantics but does not participate in ownership management.

Copyable, non-movable, and copies the resource held by ``Handle<T>`` upon construction.

## Project Structure & Build Support

```
oath/
├── docs/
│   ├── architecture.md     # Project architecture
│   └── changelog.md        # Version changelog
├── include/
│   ├── oath20/             # C++20 implementation (mainline, currently named oath)
│   ├── oath17/             # C++17 implementation (not yet supported)
│   ├── oath14/             # C++14 implementation (not yet supported)
│   └── oath.hpp            # Auto version adapter header
├── test/                   # Unit tests
├── examples/               # Sample programs
└── CMakeLists.txt          # Multi-version build script (not yet supported)
```

## Usage Example

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
    file.return(); // return the handle, or the handle won't go back to Owned<File>.
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

## Project Advantages

- **Performance**: Zero abstraction cost, close to raw pointer semantics; supports compile-time optimization.

- **Safety**: Explicit ownership, preventing typical issues like dangling pointers, double frees, and resource leaks.

- **Integrability**: Compatible with STL, raw pointers, and existing code; low adaptation cost.

- **Learning Curve**: Clear semantics, no macro magic, aligns with modern C++ usage habits.

- **Backward Compatibility**: Supports C++14~20; low migration cost across projects.

## Use Cases

- Safe alternatives to semantically ambiguous use cases of ``unique_ptr``, ``shared_ptr``, etc.

- Explicit transfer boundaries when exposing interface resources (e.g., library functions/callback parameters).

- Module-level component isolation to prevent out-of-bound pointer access.

- Resource management refactoring for legacy projects with progressive fixes.

## Roadmap

- ⏳ Complete the minimum viable implementation

- ⏳ Complete support for major standards: C++14/17/20

- ⏳ Provide STL-compatible type adapters

- ⏳ Introduce shared and weak submodules (based on safety foundation)

- ⏳ Add compile-time static checking (diagnose unmanaged raw resources)

- ⏳ Wrap standard containers with ownership-aware adapters

- ⏳ Improve interoperability testing with Boost and Abseil

## License & Contribution

Oath is released under the MIT license. Enterprises and individuals are encouraged to adopt it in refactoring, auditing, and modernization.

Feel free to submit Issues, PRs, and suggestions on GitHub.

## Contact Us

Got issues? Open one.

## Appendix: Naming Convention Quick View

|Semantics|Type Name|Description|
|--|--|--|
|Raw pointer wrapper|``Handle<T>``|Exportable from ``Owned<T>`` etc., used to construct Borrowed<T> and others|
|Owns resource|``Owned<T>``|Has lifecycle, responsible for resource release|
|Borrows resource|``Borrowed<T>``|Refers to resource owned by another, not destructible, ownership must be returned manually|
|Value copy|``Value<T>``|Does not own resource, only copies value|
|Exclusively owns resource|``Unique<T>``|Has exclusive lifecycle, automatically releases|
|Shared resource|``Shared<T>``|One writable + multiple readonly sharing, auto-release when refcount = 0|
|Observing reference|``Weak<T>``|Not involved in reference counting|

> Oath is not here to replace the language, nor to lecture developers—it’s here to give modern C++ a visible, tangible future of memory safety, with minimal barriers.