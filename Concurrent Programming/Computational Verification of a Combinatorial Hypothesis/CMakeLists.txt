cmake_minimum_required(VERSION 3.12)

project(multiset LANGUAGES C)

# set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-uninitialized -Wno-unused-function -Werror)
add_compile_options(-pthread)
add_link_options(-pthread)

# add_compile_options(-fsanitize=thread)
# add_link_options(-fsanitize=thread)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-march=native -O3)
endif()

# Optionally: include-what-you-use to minimize #include-s.
# find_program(IWYU_PATH NAMES include-what-you-use iwyu)
# if(IWYU_PATH)
#     set(CMAKE_C_INCLUDE_WHAT_YOU_USE ${IWYU_PATH})
# else()
#     message(WARNING "Could not find the program include-what-you-use / iwyu")
# endif()


# add_compile_options(-DLOG_SUMSET=1)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(common)
add_subdirectory(reference)
add_subdirectory(nonrecursive)
add_subdirectory(parallel)