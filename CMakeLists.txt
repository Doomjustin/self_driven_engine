cmake_minimum_required(VERSION 3.29)

project(CppTemplate
    VERSION 0.0.1
    LANGUAGES C CXX
)

include(CTest)
include(CMakeDependentOption)

cmake_dependent_option(${PROJECT_NAME}_BUILD_TESTS
    "Enable ${PROJECT_NAME} project tests targets" ON
    "BUILD_TESTING" OFF
)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(ConfigSafeGuards)
include(Colors)
include(Warnings)
include(LTO)
include(CopyDllsForDebug)

find_lto(CXX)

include(InstallGSL)
include(InstallFmt)
include(InstallSpdlog)
# include(InstallCatch2)
include(InstallDoctest)
include(InstallPybind11)

add_subdirectory(app)
add_subdirectory(src)
