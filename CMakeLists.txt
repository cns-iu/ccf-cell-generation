cmake_minimum_required(VERSION 3.9)
project(CELL)

# set variables
set(CMAKE_BUILD_TYPE "Release")

# find packages
find_package(CGAL REQUIRED)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.45.0 COMPONENTS filesystem)

add_executable(cell_generator main.cpp cell_generation.cpp)
target_include_directories(cell_generator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cell_generator CGAL::CGAL)

add_executable(cell_generator_imperfect imperfect_mesh.cpp cell_generation.cpp)
target_include_directories(cell_generator_imperfect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cell_generator_imperfect CGAL::CGAL)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    target_link_libraries(cell_generator ${Boost_LIBRARIES})
    target_link_libraries(cell_generator_imperfect ${Boost_LIBRARIES})
endif()


add_executable(merger merge.cpp cell_generation.cpp)
target_include_directories(merger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(merger CGAL::CGAL)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    target_link_libraries(merger ${Boost_LIBRARIES})
endif()