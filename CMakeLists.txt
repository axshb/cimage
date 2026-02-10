cmake_minimum_required(VERSION 3.15)
project(ComfyExtractor)

set(CMAKE_CXX_STANDARD 20)

# find json lib
find_package(nlohmann_json REQUIRED)

# define core lib shared by app and tests
add_library(extractor_lib 
    src/comfy_utils.cpp 
    src/png_utils.cpp
)
target_include_directories(extractor_lib PUBLIC src)
target_link_libraries(extractor_lib PUBLIC nlohmann_json::nlohmann_json)

# define main exec
add_executable(cimage src/main.cpp)
target_link_libraries(cimage PRIVATE extractor_lib)

# 3. define test exec
add_executable(unit_tests tests/test_main.cpp)
target_link_libraries(unit_tests PRIVATE extractor_lib)