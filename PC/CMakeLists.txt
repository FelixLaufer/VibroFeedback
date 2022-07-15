cmake_minimum_required(VERSION 3.14)

project(VibroControl)

find_package(Boost 1.46.1 REQUIRED system filesystem)

include_directories(.)
add_subdirectory(VibroControl)
add_subdirectory(apps)
