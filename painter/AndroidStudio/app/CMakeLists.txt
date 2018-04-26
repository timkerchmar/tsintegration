cmake_minimum_required(VERSION 3.4.1)

add_library(app-glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O3")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-lm_hard")

file(GLOB TSSHARED_CPP ../../../lib/*.cpp)
file(GLOB GAME_CPP ../../*.cpp)
file(GLOB ANDROID_CPP ../../../lib/android/*.cpp)

include_directories(../../
                    ../../../lib/
                    ../../../lib/android/)

add_definitions(-DHAVE_NEON=1)

add_library(native-activity SHARED
            ${ANDROID_CPP}
            ${TSSHARED_CPP}
            ${GAME_CPP})

target_include_directories(native-activity PRIVATE
                           ${ANDROID_NDK}/sources/android/native_app_glue)

# add lib dependencies
target_link_libraries(native-activity
                      android
                      app-glue
                      log
                      EGL
                      GLESv3)
