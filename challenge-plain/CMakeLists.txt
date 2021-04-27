# Simple CMake project, builds cross-platform with only libjpeg as a dependency
cmake_minimum_required (VERSION 3.5)

project ("challenge-plain")
# we want C++17 features (filesystem, etc.)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The main executable and the sources we're compiling.
add_executable (challenge-plain
                    "main.cpp"
                    "imagebuffer.cpp"
                    "tveffect.cpp"
               )

# Add libjpeg(-turbo), assuming the library is already installed.
# On Windows, I used vcpkg.exe to install the library with the following command:
# `vcpkg.exe install libjpeg-turbo:x64-windows`
# To install vcpkg.exe: https://docs.microsoft.com/en-us/cpp/build/install-vcpkg?view=msvc-160&tabs=windows
find_package(JPEG REQUIRED)
target_link_libraries(challenge-plain PRIVATE ${JPEG_LIBRARIES})
target_include_directories(challenge-plain PRIVATE ${JPEG_INCLUDE_DIR})
