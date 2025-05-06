include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.2
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)


set(SPDLOG_FMT_EXTERNAL ON)
set(SPDLOG_BUILD_EXAMPLE OFF)

FetchContent_MakeAvailable(spdlog)
