#include <catch2/catch_test_macros.hpp>

int main()
{
    int failed = 0;

    for (const auto& test : mini_catch::registry())
    {
        try
        {
            test.function();
            std::cout << "[OK] " << test.name << '\n';
        }
        catch (const std::exception& e)
        {
            ++failed;
            std::cerr << "[FAIL] " << test.name << ": " << e.what() << '\n';
        }
        catch (...)
        {
            ++failed;
            std::cerr << "[FAIL] " << test.name << ": unknown exception\n";
        }
    }

    if (failed != 0)
    {
        std::cerr << failed << " test(s) failed\n";
        return 1;
    }

    return 0;
}
