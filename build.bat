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

if %* != "Release" (
    ECHO found
)
echo "Building engine..."

if not exist "build\NUL" mkdir "build"
cd build

cmake -DASSIMP_BUILD_TESTS=OFF^
	  -DASSIMP_BUILD_ASSIMP_TOOLS=OFF^
	  -DBUILD_BULLET2_DEMOS=OFF^
	  -DBUILD_BULLET_ROBOTICS_EXTRA=OFF^
	  -DBUILD_BULLET_ROBOTICS_GUI_EXTRA=OFF^
	  -DBUILD_CPU_DEMOS=OFF^
	  -DBUILD_OPENGL3_DEMOS=OFF^
	  -DBUILD_TESTING=OFF^
	  -DBUILD_UNIT_TESTS=OFF^
	  -DYAML_CPP_BUILD_TOOLS=OFF^
	  -DUSE_GLUT=OFF ..
cmake --build . --config %*

echo "Build succeeded..."
echo "Lunching... "

cd Exe\%*\
start AdHoc.exe

cd ..
