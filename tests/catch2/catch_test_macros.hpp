#ifndef LOCAL_CATCH_TEST_MACROS_HPP
#define LOCAL_CATCH_TEST_MACROS_HPP

#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace mini_catch
{
    struct TestCase
    {
        const char* name;
        void (*function)();
    };

    inline std::vector<TestCase>& registry()
    {
        static std::vector<TestCase> tests;
        return tests;
    }

    struct Registrar
    {
        Registrar(const char* name, void (*function)())
        {
            registry().push_back({name, function});
        }
    };

    inline void require(bool condition,
                        const char* expression,
                        const char* file,
                        int line)
    {
        if (!condition)
        {
            std::ostringstream out;
            out << file << ':' << line << ": REQUIRE(" << expression << ") failed";
            throw std::runtime_error(out.str());
        }
    }
}

#define MINI_CATCH_JOIN_IMPL(lhs, rhs) lhs##rhs
#define MINI_CATCH_JOIN(lhs, rhs) MINI_CATCH_JOIN_IMPL(lhs, rhs)

#define TEST_CASE(name)                                                        \
    static void MINI_CATCH_JOIN(test_function_, __LINE__)();                   \
    static ::mini_catch::Registrar MINI_CATCH_JOIN(test_registrar_, __LINE__)( \
        name, MINI_CATCH_JOIN(test_function_, __LINE__));                      \
    static void MINI_CATCH_JOIN(test_function_, __LINE__)()

#define REQUIRE(expression)                                                     \
    ::mini_catch::require(static_cast<bool>(expression), #expression, __FILE__, __LINE__)

#define CHECK(expression) REQUIRE(expression)

#endif
