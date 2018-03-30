CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(zombiroid)

SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -F /Library/Frameworks/SDL2.framework/Headers -framework SDL2 -W -Wall -std=c++11 -o zombiroidgame")
SET(PROJECT_SOURCE_DIR "/Users/nebelwerferg/zombiroid/")
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
INCLUDE(FindSDL2)
SET(SOURCE_FILES game.cpp)
# I installed SDL2 with Homebrew on my Mac, that's why they living in /usr/local/Cellar/ .
SET(SDL2_INCLUDE_DIR "/usr/local/Cellar/sdl2/2.0.8/include/SDL2/")
SET(SDL2_LINK_DIR "/usr/local/Cellar/sdl2/2.0.8/lib/")
# Not sure which one is working - Homebrew one or manual one, just keep this and detect afterwards.
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR})
LINK_DIRECTORIES(${SDL2_LINK_DIR})

ADD_EXECUTABLE(zombiroidgame ${SOURCE_FILES})

ADD_LIBRARY(zombiroid ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(zombiroid "${SDL2_LINK_DIR}/libSDL2.dylib")

