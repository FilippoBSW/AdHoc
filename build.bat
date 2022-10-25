::*********************************************************************************
::MIT License
::
::Copyright(c) 2021 - 2022 Filippo - BSW
::
::Permission is hereby granted, free of charge, to any person obtaining a copy
::of this software and associated documentation files(the "Software"), to deal
::in the Software without restriction, including without limitation the rights
::to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
::copies of the Software, and to permit persons to whom the Software is
::furnished to do so, subject to the following conditions:
::
::The above copyright notice and this permission notice shall be included in all
::copies or substantial portions of the Software.
::
::THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
::IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
::FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
::AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
::LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
::OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
::SOFTWARE.
::*********************************************************************************

echo "Building engine..."

if not exist "build\NUL" mkdir "build"
cd build

cmake -DUSE_PLATFORM="Windows"^
	  -DCMAKE_BUILD_TYPE=%*^
	  -DASSIMP_BUILD_TESTS=OFF^
	  -DASSIMP_BUILD_ASSIMP_TOOLS=OFF^
	  -DBUILD_TESTING=OFF^
        -DALSOFT_EXAMPLES=OFF^
        -DALSOFT_INSTALL_EXAMPLES=OFF^
        -DSDL2MAIN_LIBRARY=""^
        -DSDL2_CORE_LIBRARY=""^
        -DSDL2_INCLUDE_DIR=""^
	  -DYAML_CPP_BUILD_TOOLS=OFF ..
cmake --build . --config %*

echo "Build succeeded..."
echo "Lunching... "

cd Exe\%*\
start AdHoc.exe

cd ..
