# 启用 C++ 代码格式检查
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    message(STATUS "Found clang-format: ${CLANG_FORMAT}")
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES 
        ${PROJECT_SOURCE_DIR}/app/*.cpp 
        ${PROJECT_SOURCE_DIR}/src/*.cpp 
        ${PROJECT_SOURCE_DIR}/src/*.h 
        ${PROJECT_SOURCE_DIR}/src/*.cppm
        ${PROJECT_SOURCE_DIR}/include/storm/*.h 
    )
    add_custom_target(
        clangformat
        COMMAND ${CLANG_FORMAT} -i ${ALL_CXX_SOURCE_FILES}
        COMMENT "Running clang-format on all source files"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
endif()