add_library(sde_collection STATIC)

add_library(sde::collection ALIAS sde_collection)

target_sources(sde_collection
    PUBLIC FILE_SET CXX_MODULES
    FILES
        vector.cppm
)

target_compile_features(sde_collection PRIVATE cxx_std_23)

target_link_libraries(sde_collection
    PRIVATE
        doctest::doctest
)

if (${PROJECT_NAME}_BUILD_TESTS)
    add_subdirectory(test)
endif()