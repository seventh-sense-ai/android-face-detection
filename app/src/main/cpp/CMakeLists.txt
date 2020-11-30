# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.


add_library( # Sets the name of the library.
        seventh-sense-ai-facedetection

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        facedetector.cpp
        seventh-sense-ai-facedetection.cpp)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.


find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

add_library(
        facedetection
        STATIC
        IMPORTED
)

set_target_properties(
        facedetection
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}/libfacedetection.a
)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_include_directories(seventh-sense-ai-facedetection PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
        $<INSTALL_INTERFACE:includes>  # <prefix>/include/mylib
        )

set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED)

target_link_libraries( # Specifies the target library.
        seventh-sense-ai-facedetection
        jnigraphics
        android
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${OpenCV_LIBS}
        facedetection
        -static-openmp -fopenmp)
