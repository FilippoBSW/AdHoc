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
// @brief	DistributionGGX.
// 
//************************************************************************
float DistributionGGX(float NdotH, float roughness) {
	float alpha  = roughness * roughness;
	float alpha2 = alpha * alpha;

	float denom = NdotH * NdotH * (alpha2 - 1.0f) + 1.0f;
	denom = PI * denom * denom;

	return alpha2 / max(denom, 0.0000001f);
}

//************************************************************************
// @brief	GeometrySmith.
// 
//************************************************************************
float GeometrySmith(float NdotV, float NdotL, float roughness) {
	float r = roughness + 1.0f;
	float k = (r * r) / 8.0f;

	float ggx1 = NdotV / (NdotV * (1.0f - k) + k);
	float ggx2 = NdotL / (NdotL * (1.0f - k) + k);

	return ggx1 * ggx2;
}

//************************************************************************
// @brief	FresnelSchlick.
// 
//************************************************************************
vec3 FresnelSchlick(float NdotV, vec3 baseReflectivity) {
	return baseReflectivity + (1.0f - baseReflectivity) * pow(1.0f - NdotV, 5.0f);
}
