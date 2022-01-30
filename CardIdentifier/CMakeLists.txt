 # CMakeList.txt : CMake project for CardIdentifier, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.

set( OpenCV_DIR E:/Documents/opencv/build/install/x64/vc16/lib )

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable (CardIdentifier "main.cpp" "CardIdentifier.h")

target_link_libraries( CardIdentifier ${OpenCV_LIBS} )

# TODO: Add tests and install targets if needed.
