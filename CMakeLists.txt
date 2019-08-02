
cmake_minimum_required (VERSION 3.10)
project(
    SysCrypto
    VERSION 0.1.0
    DESCRIPTION "Header-only wrapper library of platform OS specific Cryptographic API"
    LANGUAGES CXX
)

if(MSVC)
    add_compile_options("/Zc:__cplusplus")
endif()
    
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)



add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE  "include/")

if(WIN32)
    target_link_libraries(${PROJECT_NAME} INTERFACE  "Crypt32.lib")
endif()


#add_executable (test WIN32  "${CMAKE_SOURCE_DIR}/test/test.cpp")
add_executable (test "${CMAKE_SOURCE_DIR}/test/test.cpp")
target_link_libraries(test ${PROJECT_NAME})
