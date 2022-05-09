// *********************************************************************************
// MIT License
//
// Copyright (c) 2022 Filippo-BSW
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// *********************************************************************************

#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive    : enable

//************************************************************************
// @brief	Constants.
// 
//************************************************************************
const float PI = 3.14159265359;

//************************************************************************
// @brief	Data structures.
// 
//************************************************************************
struct Attenuation {
	float constant;
	float linear;
	float quadratic;
};

struct Material {
	float roughness;
	float metallicness;
	float transparency;
	vec3  albedo;
};

struct DirectionalLight {
	vec3  direction;
	vec3  color;
	float intensity;
	int   castsShadow;
	int   samplerId;
};

struct PointLight {
	float       intensity;
	vec3        position;
	vec3        color;
	Attenuation attenuation;
};

struct SpotLight {
	vec3        position;
	vec3        color;
	Attenuation attenuation;

	vec3  direction;
	float coneSize;		 // Phi
	float smoothness;	 // Gamma
	float intensity;
};
