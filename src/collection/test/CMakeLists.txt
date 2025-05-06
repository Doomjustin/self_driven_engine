function(add_collection_test name)
    add_executable(collction.test.${name} test_${name}.cpp)

    target_link_libraries(collction.test.${name}
        PRIVATE
            sde::collection
            doctest::doctest
    )

    target_compile_features(collction.test.${name} PRIVATE cxx_std_23)

endfunction()


add_collection_test(vector)