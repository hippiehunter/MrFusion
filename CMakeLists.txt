project(mrfusion)
cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE mrfusion_SRC 
    "src/*.cpp")

include_directories("inc")

FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()

ADD_DEFINITIONS("-std=c++0x")

add_executable(mrfusion ${mrfusion_SRC})