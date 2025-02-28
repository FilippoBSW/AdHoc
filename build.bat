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
