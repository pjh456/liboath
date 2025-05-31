#pragma once

#include <string>
#include <stdexcept>

namespace oath::detail
{
    [[noreturn]]
    inline void runtime_check_failed(const char* expr, const char* file, int line)
    {
        throw std::runtime_error
        (
            std::string("OATH runtime check failed: ") + expr 
            + " at " + file + ":" + std::to_string(line)
        );
    }

    [[noreturn]]
    inline void runtime_throw_exception(const std::exception& e, const char* file, const char* func)
    {
        throw std::runtime_error
        (
            std::string("OATH runtime exception: ") + e.what()
            + " at " + file + ":" + func
        );
    }
}