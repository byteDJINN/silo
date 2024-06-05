cmake_minimum_required(VERSION 3.10)

# Set the project name
project(silo)

# Add the FTXUI library
add_subdirectory(./deps/FTXUI ftxui)

# Add the executable
add_executable(silo main.cpp)

# Link the FTXUI library
target_link_libraries(silo ftxui::screen ftxui::dom ftxui::component)
