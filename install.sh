cmake -DCMAKE_BUILD_TYPE=$1\
	 '-DCMAKE_OSX_ARCHITECTURES=arm64;x86_64'\
	  -DASSIMP_BUILD_TESTS=OFF\
	  -DASSIMP_BUILD_ASSIMP_TOOLS=OFF\
	  -DBUILD_BULLET2_DEMOS=OFF\
	  -DBUILD_BULLET_ROBOTICS_EXTRA=OFF\
	  -DBUILD_BULLET_ROBOTICS_GUI_EXTRA=OFF\
	  -DBUILD_CPU_DEMOS=OFF\
	  -DBUILD_OPENGL3_DEMOS=OFF\
	  -DBUILD_TESTING=OFF\
	  -DBUILD_UNIT_TESTS=OFF\
	  -DYAML_CPP_BUILD_TOOLS=OFF\
	  -DUSE_GLUT=OFF\
	  -DPX_GENERATE_STATIC_LIBRARIES=ON\
	  '-DPHYSX_CXX_FLAGS=-w -arch arm64' ..
cmake --build .
open App/AdHoc.app
