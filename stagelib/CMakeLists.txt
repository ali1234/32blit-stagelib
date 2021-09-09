file (GLOB SOURCES src/*.cpp)
add_library(StageLib OBJECT ${SOURCES})
target_link_libraries(StageLib BlitEngine)
target_include_directories(StageLib PUBLIC src)
