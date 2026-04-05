cmake_minimum_required(VERSION 3.15)
project(BoidsSimulation VERSION 1.0 DESCRIPTION "Moteur de simulation de Boids")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3.0.2 COMPONENTS Graphics Window System REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(BoidSimulate ${SOURCES})

target_link_libraries(BoidSimulate PRIVATE 
    SFML::Graphics 
    SFML::Window 
    SFML::System
    Vulkan::Vulkan
)

target_include_directories(BoidSimulate PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(BoidSimulate PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor)