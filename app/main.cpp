#include <fmt/base.h>
#include <spdlog/spdlog.h>

#include <cstdlib>

int main(int argc, char* argv[])
{
    fmt::print("Hello, {}!\n", "world");

    return EXIT_SUCCESS;
}
