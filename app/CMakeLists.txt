add_executable(app main.cpp)

target_link_libraries(app 
    PRIVATE 
        fmt::fmt
        spdlog::spdlog
        doctest
)

target_compile_features(app PRIVATE cxx_std_23)

target_set_warnings(app ENABLE ALL AS_ERROR ALL DISABLE Annoying)
target_enable_lto(app optimized)
copy_dlls_for_debug(app "" "")
