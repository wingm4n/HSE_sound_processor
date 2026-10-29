cmake_minimum_required(VERSION 3.14)
project(sound_processor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)  # GNU extensions: M_PI, getopt_long, packed structs

option(ENABLE_SANITIZERS "Build with AddressSanitizer/LeakSanitizer" ON)
option(BUILD_TESTING "Build unit tests (fetches Catch2)" ON)

add_compile_options(-Wall -Wextra)
if(ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address -fsanitize=leak -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address -fsanitize=leak)
endif()

add_library(sp_lib STATIC
    src/argsparser.cpp
    src/application.cpp
    src/wavestream.cpp
    src/filters.cpp
    src/pipeline.cpp
)
target_include_directories(sp_lib PUBLIC include)

add_executable(sound_processor main.cpp)
target_link_libraries(sound_processor PRIVATE sp_lib)

if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.2)
    FetchContent_MakeAvailable(Catch2)

    add_executable(test_sound_processor tests/test_sound_processor.cpp)
    target_link_libraries(test_sound_processor PRIVATE sp_lib Catch2::Catch2WithMain)
endif()
