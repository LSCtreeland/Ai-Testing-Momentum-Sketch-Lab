cmake_minimum_required(VERSION 3.10)
project(MomentumSketchUnitTest LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Momentum-Sketch/CPU/Common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Momentum-Sketch/CPU/Struct)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Momentum-Sketch/CPU/Src)

add_definitions(-O3)

enable_testing()

# 1. Hash Uniformity Test
add_executable(test_hash_uniformity test_hash_uniformity.cpp ../Momentum-Sketch/CPU/Common/murmur3.c)
add_test(NAME HashUniformityTest COMMAND test_hash_uniformity)

# 2. TUPLES Operators Test
add_executable(test_tuples_operators test_tuples_operators.cpp ../Momentum-Sketch/CPU/Common/murmur3.c)
add_test(NAME TuplesOperatorsTest COMMAND test_tuples_operators)

# 3. MomentumSketch Boundary Test
add_executable(test_momentum_sketch_boundary test_momentum_sketch_boundary.cpp ../Momentum-Sketch/CPU/Common/murmur3.c)
add_test(NAME MomentumSketchBoundaryTest COMMAND test_momentum_sketch_boundary)
