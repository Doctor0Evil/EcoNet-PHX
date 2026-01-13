cmake_minimum_required(VERSION 3.15)
project(EcoNetPHXCorridorSynergy CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(econet_corridor
    src/corridor/CorridorSynergyAmplifier.cpp
    src/corridor/CorridorAnalyticsService.cpp
)

target_include_directories(econet_corridor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(test_corridor_synergy
    tests/test_corridor_synergy_amplifier.cpp
)

target_include_directories(test_corridor_synergy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(test_corridor_synergy
    econet_corridor
)
