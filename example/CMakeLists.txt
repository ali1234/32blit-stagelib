# Basic parameters; check that these match your project / environment
cmake_minimum_required (VERSION 3.8)

project (stagelibexample)

# Build configuration; approach this with caution!
if (MSVC)
    add_compile_options ("/W4" "/wd4244" "/wd4324")
else ()
    add_compile_options ("-Wall" "-Wextra" "-Wdouble-promotion")
endif ()

find_package (32BLIT CONFIG REQUIRED PATHS ../../32blit-sdk)
find_package (StageLib CONFIG REQUIRED PATHS ..)

file (GLOB SOURCES src/*.cpp)

blit_executable (${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} StageLib)
blit_metadata (${PROJECT_NAME} metadata.yaml)
add_custom_target (flash DEPENDS ${PROJECT_NAME}.flash)

# setup release packages
install (FILES ${PROJECT_DISTRIBS} DESTINATION .)
set (CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set (CPACK_GENERATOR "ZIP" "TGZ")
include (CPack)
