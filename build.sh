#!/usr/bin/env bash

#*********************************************************************************
#MIT License
#
#Copyright(c) 2021 - 2022 Filippo - BSW
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files(the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.
#*********************************************************************************

function build {
    echo "Building engine..."

    mkdir -p build
    cd build

    if [ "$1" == "Mac" ]; then
      USE_PLATFORM="Mac"
      cmake -DUSE_PLATFORM=$USE_PLATFORM\
		  -DCMAKE_BUILD_TYPE=$2\
	    '-DCMAKE_OSX_ARCHITECTURES=x86_64'\
	    -DASSIMP_BUILD_TESTS=OFF\
	    -DASSIMP_BUILD_ASSIMP_TOOLS=OFF\
		  -DCMAKE_CXX_FLAGS=-w\
		  -DCMAKE_C_FLAGS=-w\
	    -DYAML_CPP_BUILD_TOOLS=OFF\
		  -DBUILD_TESTING=OFF\
      -DALSOFT_EXAMPLES=OFF\
      -DALSOFT_INSTALL_EXAMPLES=OFF\
      -DSDL2MAIN_LIBRARY=""\
      -DSDL2_CORE_LIBRARY=""\
      -DSDL2_INCLUDE_DIR=""\
	    '-DPHYSX_CXX_FLAGS=-w' ..
    elif [ "$1" == "Linux" ]; then
      USE_PLATFORM="Linux"
      CXX=clang++ cmake -DUSE_PLATFORM=$USE_PLATFORM\
		  -DCMAKE_BUILD_TYPE=$2\
	    -DASSIMP_BUILD_TESTS=OFF\
	    -DASSIMP_BUILD_ASSIMP_TOOLS=OFF\
	    -DYAML_CPP_BUILD_TOOLS=OFF\
		  -DBUILD_TESTING=OFF\
      -DALSOFT_EXAMPLES=OFF\
      -DALSOFT_INSTALL_EXAMPLES=OFF\
      -DSDL2MAIN_LIBRARY=""\
      -DSDL2_CORE_LIBRARY=""\
      -DSDL2_INCLUDE_DIR=""\
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
