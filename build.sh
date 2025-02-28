#!/usr/bin/env bash

function build {
    echo "Building engine..."

    mkdir -p build
    cd build

    if [ "$1" == "Mac" ]; then
        USE_PLATFORM="Mac"
        cmake -DUSE_PLATFORM=$USE_PLATFORM       \
		      -DCMAKE_BUILD_TYPE=$2              \
	          '-DCMAKE_OSX_ARCHITECTURES=x86_64' \
	          -DASSIMP_BUILD_TESTS=OFF           \
	          -DASSIMP_BUILD_ASSIMP_TOOLS=OFF    \
		      -DCMAKE_CXX_FLAGS=-w               \
		      -DCMAKE_C_FLAGS=-w                 \
	          -DYAML_CPP_BUILD_TOOLS=OFF         \
		      -DBUILD_TESTING=OFF                \
              -DALSOFT_EXAMPLES=OFF              \
              -DALSOFT_INSTALL_EXAMPLES=OFF      \
              -DSDL2MAIN_LIBRARY=""              \
              -DSDL2_CORE_LIBRARY=""             \
              -DSDL2_INCLUDE_DIR=""              \
	          '-DPHYSX_CXX_FLAGS=-w' ..
    elif [ "$1" == "Linux" ]; then
        USE_PLATFORM="Linux"
        CXX=clang++ cmake -DUSE_PLATFORM=$USE_PLATFORM \
		            -DCMAKE_BUILD_TYPE=$2              \
	                -DASSIMP_BUILD_TESTS=OFF           \
	                -DASSIMP_BUILD_ASSIMP_TOOLS=OFF    \
	                -DYAML_CPP_BUILD_TOOLS=OFF         \
		            -DBUILD_TESTING=OFF                \
                    -DALSOFT_EXAMPLES=OFF              \
                    -DALSOFT_INSTALL_EXAMPLES=OFF      \
                    -DSDL2MAIN_LIBRARY=""              \
                    -DSDL2_CORE_LIBRARY=""             \
                    -DSDL2_INCLUDE_DIR=""              \
	                '-DPHYSX_CXX_FLAGS=-w' ..
    fi

    cmake --build .

    echo "Build succeeded..."
    echo "Lauching..."

    if [ "$1" == "Mac" ]; then
        open App/AdHoc.app
    elif [ "$1" == "Linux" ]; then
        Exe/AdHoc
    fi

    cd ..
}

if [ "$1" != "Mac" ] && [ "$1" != "Linux" ]; then
  echo "Valid Usage: ./build.sh <platform> <build type>"
  echo "             <platform>   - Mac | Linux"
  echo "             <build type> - Release | Debug"
else
  build $1 $2
fi
