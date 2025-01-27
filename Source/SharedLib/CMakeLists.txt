cmake_minimum_required(VERSION 3.16)

message(STATUS "Configuring Amalgam Engine Shared Library")

# Add our static library target.
add_library(SharedLib STATIC "")

# Provide an alias that can be used whether the target comes from 
# add_subdirectory() or find_package().
add_library(AmalgamEngine::SharedLib ALIAS SharedLib)

target_include_directories(SharedLib
    PUBLIC
        ${SDL2_INCLUDE_DIRS}
        ${SDL2PP_INCLUDE_DIRS}
)

target_link_libraries(SharedLib
    PUBLIC
        ${SDL2_LIBRARIES}
        ${SDL2PP_LIBRARIES}
        SDL2_net-static
        SDL2pp
        lz4_static
        EnTT::EnTT
        Bitsery::bitsery
        nlohmann_json::nlohmann_json
        Tracy::TracyClient
)

# Use the config override path, if one was provided.
# Note: This propagates to ClientLib, ServerLib, etc (since they include 
#       SharedLib and the compile option is public).
if (NOT AM_CONFIG_OVERRIDE_PATH STREQUAL "")
    target_include_directories(SharedLib PUBLIC ${AM_CONFIG_OVERRIDE_PATH})
    target_compile_options(SharedLib PUBLIC -DAM_OVERRIDE_DEFAULT_CONFIGS)
endif()

# Compile with C++20.
target_compile_features(SharedLib PRIVATE cxx_std_20)
set_target_properties(SharedLib PROPERTIES CXX_EXTENSIONS OFF)

# Enable compile warnings.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(SharedLib PUBLIC -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(SharedLib PUBLIC /W3 /permissive-)
endif()

# Build all of the subdirectories.
add_subdirectory(Config)
add_subdirectory(Messages)
add_subdirectory(Network)
add_subdirectory(Renderer)
add_subdirectory(Simulation)
add_subdirectory(Utility)
