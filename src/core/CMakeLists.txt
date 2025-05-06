configure_file(config.cppm.in ${CMAKE_CURRENT_LIST_DIR}/config.cppm @ONLY)

add_library(sde_core STATIC)

add_library(sde::core ALIAS sde_core)

target_sources(sde_core
    PUBLIC FILE_SET CXX_MODULES
    FILES
        config.cppm
)

target_link_libraries(sde_core
    PUBLIC
        Microsoft.GSL::GSL
        fmt::fmt
        spdlog::spdlog
        doctest::doctest
)

target_compile_features(sde_core PUBLIC cxx_std_23)

target_compile_definitions(sde_core
    INTERFACE
        $<$<CONFIG:Debug>:SDE_DEBUG>
        $<$<CONFIG:Release>:DOCTEST_CONFIG_DISABLE>
)
