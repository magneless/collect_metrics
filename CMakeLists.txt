cmake_minimum_required(VERSION 3.10)
project(Metrics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(metrics
  metrics/metrics.cpp
  metrics/metrics.hpp
)

target_include_directories(metrics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics
)

add_executable(main_exec
  main.cpp
)

target_link_libraries(main_exec PRIVATE
    metrics
    Threads::Threads
)
