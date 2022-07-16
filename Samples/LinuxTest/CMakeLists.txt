cmake_minimum_required(VERSION 3.21)

add_executable(DaxaSamples_LinuxTest
    "main.cpp"
)

find_package(glm CONFIG REQUIRED)
target_link_libraries(DaxaSamples_LinuxTest PUBLIC daxa::daxa glm::glm)

set_project_warnings(DaxaSamples_LinuxTest)
