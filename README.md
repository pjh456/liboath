# Oath: A Safer Ownership-Oriented Abstraction for Modern C++

[中文版文档](docs/README-zh.md) • [English Docs (coming soon)](docs/README-en.md)

## Project Introduction

**Oath** is a lightweight and low-abstraction-cost ownership modeling library for modern C++. It brings Rust-style resource safety and composability to C++ while preserving full compatibility with native STL and existing third-party codebases.

Unlike systems with garbage collection or invasive syntax rewrites, **Oath** offers a progressive model: explicit ownership semantics with expressive and safe APIs, designed to work *with* existing C++ idioms — not against them.

Built with C++20 as the primary target and offering compatible subsets for C++17 and C++14, Oath provides a practical and high-performance toolkit for large-scale engineering, legacy refactoring, and modern idiomatic C++ design.

## Key Features

- ✅ Lightweight abstraction with zero-overhead in optimized builds
- ✅ Clear separation of *ownership*, *borrowing*, *viewing* semantics
- ✅ Value-based `Result<T, E>` and `Option<T>` types for composable error handling
- ✅ Implicit ownership routing — user ergonomics without runtime GC
- ✅ Safe interop with STL containers and raw pointers
- ✅ Easily adoptable in both new and legacy code

## Example

```cpp
#include <string>
#include <iostream>
#include "oath.hpp"

using namespace std;
using namespace oath;

class File {
private:
    char* data;
    size_t length;

public:
    File(const char* file_path);
    File(const File& other); // Enables use with Value<File>
    ~File();

    static File* open(const char* file_path);
    bool is_open() const;     // Accessible via Readonly<File>
    void copy_self() noexcept; // Requires mutable access (Borrowed or Owned)
};

Result<bool, string> checkFileAvailable(Readonly<File> file) {
    try {
        return Result<bool, string>::ok(file.is_open());
    } catch (...) {
        return Result<bool, string>::err("File open error!");
    }
}

void handleFileData(Borrowed<File> file) {
    file.copy_self();
    file.return_handle(); // Return handle back to Owned<File>
}

void saveFile(Value<File> file) {
    // Receives a copy of File, e.g., for serialization
}

int main() {
    Owned<File> file = make_owned(File::open("example.txt"));

    auto result = checkFileAvailable(file);
    auto ok_handler = [](bool available) { return available; };
    auto err_handler = [](string&& err) { return false; };

    bool is_ok = result.match(ok_handler, err_handler);
    cout << (is_ok ? "available" : "unavailable") << endl;

    handleFileData(file.borrow()); // Mutably borrow the file (non-owning)

    saveFile(file); // Copy the file value to another handler

    return 0;
}
```