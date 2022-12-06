cmake_minimum_required(VERSION 3.16)
project(Geant4MWE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE})
include_directories(${Geant4_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/src)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/src/*.hpp)

add_executable(g4mwe ${sources} ${headers})
target_link_libraries(g4mwe ${Geant4_LIBRARIES})
