cmake_minimum_required(VERSION 3.14)

project(WassersteinKmeans)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.2
)

FetchContent_MakeAvailable(Catch2)

add_executable(main ./main.cpp)

target_include_directories(main PUBLIC includes)

set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(mini_batch_kmeans ./mini_batch_kmeans.cpp)

target_include_directories(mini_batch_kmeans PUBLIC includes)

set_target_properties(mini_batch_kmeans PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB TEST_FILES tests/test_*.cpp)

add_executable(tests ${TEST_FILES})

target_include_directories(tests PUBLIC includes)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

add_test(NAME run_tests COMMAND tests)

set_tests_properties(run_tests PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

