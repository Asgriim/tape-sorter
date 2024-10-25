project(tape_unit_tests)
include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories($ENV{HOME}/googletest/googletest/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../tapes/include/)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../tapes/src/*.cpp)
file(GLOB_RECURSE TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(tests main.cpp  ${SRC_FILES} ${TEST_FILES})
target_link_libraries(tests gtest_main)
target_compile_definitions(tests PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test_data/")
