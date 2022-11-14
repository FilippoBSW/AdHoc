// *********************************************************************************
// MIT License
//
// Copyright (c) 2021-2022 Filippo-BSW
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
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// *********************************************************************************

#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive    : enable
//#extension GL_EXT_debug_printf : enable

#include "pbr_data.glsl"
#include "pbr_functions.glsl"

 layout(location = 0) in vec3 inNormals;
 layout(location = 1) in vec3 inWorldPosition;
 layout(location = 2) in vec4 inLightPosition;

 layout(location = 0) out vec4 outFragColor;

 layout (set = 1, binding = 0) uniform Data {
	vec3 ambient;
    vec3 cameraPosition;
	int pcf2;
 } ubo;

 layout (set = 1, binding = 1) uniform DataPoint {
	DirectionalLight directionalLight;
 };

// layout (set = 1, binding = 2) uniform samplerCube cubeShadowMap[1];
layout (set = 1, binding = 2) uniform sampler2D shadowMap;

 layout(push_constant) uniform Materials {
	layout(offset = 64) Material material;
 };

vec3 CalculateDirectionalLights(vec3 N, vec3 V, vec3 reflectivity, DirectionalLight light) {
	vec3 L = normalize(light.direction);
	vec3 H = normalize(V + L);

	vec3  radiance = light.color * light.intensity;

	// Calculate dot products
	float NdotV = max(dot(N, V), 0.0000001f);
	float NdotL = max(dot(N, L), 0.0000001f);
	float HdotV = max(dot(H, V), 0.0f);
	float NdotH = max(dot(N, H), 0.0f);

	// Cook-Torrance BRDF
	float D = DistributionGGX(NdotH, material.roughness);
	float G = GeometrySmith(NdotV, NdotL, material.roughness);
	vec3  F = FresnelSchlick(HdotV, reflectivity);

	// Diffuse
	vec3 diffuse = vec3(1.0f) - F;
	diffuse     *= 1.0f - material.metallicness;

	// Specular
	vec3 specular = D * G * F;
	specular /= 4.0f * NdotV * NdotL;

	return ((diffuse * material.albedo / PI + specular) * radiance * NdotL);
}

vec3 CalculatePointLights(vec3 N, vec3 V, vec3 reflectivity, PointLight light) {
	vec3 L = normalize(light.position - inWorldPosition);
	vec3 H = normalize(V + L);

	float distance    = length(light.position - inWorldPosition);
	vec3  direction   = (light.position - inWorldPosition) / distance;
	float attenuation = 1.0f / (light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.quadratic * (distance * distance));
	vec3  radiance    = light.color * light.intensity * attenuation * max( 0.0f, dot(direction, N));

	// Calculate dot products
	float NdotV = max(dot(N, V), 0.0000001f);
	float NdotL = max(dot(N, L), 0.0000001f);
	float HdotV = max(dot(H, V), 0.0f);
	float NdotH = max(dot(N, H), 0.0f);

	// Cook-Torrance BRDF
	float D = DistributionGGX(NdotH, material.roughness);
	float G = GeometrySmith(NdotV, NdotL, material.roughness);
	vec3  F = FresnelSchlick(HdotV, reflectivity);

	// Diffuse
	vec3 diffuse = vec3(1.0f) - F;
	diffuse *= 1.0f - material.metallicness;

	// Specular
	vec3 specular = D * G * F;
	specular /= 4.0f * NdotV * NdotL;

	return (diffuse * material.albedo / PI + specular) * radiance * NdotL;
}

vec3 CalculateSpotLights(vec3 N, vec3 V, vec3 reflectivity, SpotLight light) {
	vec3 L = normalize(light.position - inWorldPosition);
	vec3 H = normalize(V + L);

	// Spot light
	float theta     = dot(L, normalize(-light.direction));
	float intensity = smoothstep(0.0f, 1.0f, (theta - light.coneSize) / light.smoothness);

	float distance    = length(light.position - inWorldPosition);
	vec3  direction   = (light.position - inWorldPosition) / distance;
	float attenuation = 1.0f / (light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.quadratic * (distance * distance));
	vec3  radiance    = light.color * light.intensity * attenuation * max(0.0f, dot(direction, N));

	// Calculate dot products
	float NdotV = max(dot(N, V), 0.0000001f);
	float NdotL = max(dot(N, L), 0.0000001f);
	float HdotV = max(dot(H, V), 0.0f);
	float NdotH = max(dot(N, H), 0.0f);

	// Cook-Torrance BRDF
	float D = DistributionGGX(NdotH, material.roughness);
	float G = GeometrySmith(NdotV, NdotL, material.roughness);
	vec3  F = FresnelSchlick(HdotV, reflectivity);

	// Diffuse
	vec3 diffuse = vec3(1.0f) - F;
	diffuse *= 1.0f - material.metallicness;

	// Specular
	vec3 specular = D * G * F;
	specular /= 4.0f * NdotV * NdotL;

	return ((diffuse * material.albedo / PI + specular) * radiance * NdotL) * intensity;
}

float ShadowCalculation(vec4 shadowCoords, int pcf, float bias) {
    vec3 projCoords    = shadowCoords.xyz / shadowCoords.w;
    float currentDepth = projCoords.z;

	if(pcf == 0){
     float closestDepth = texture(shadowMap, projCoords.xy).r;
	 return  currentDepth > closestDepth ? 1.0f : 0.0f;
	} else{

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

	float retDiv = 0.0;
	for(int x = -pcf; x <= pcf; ++x)
	{
    	for(int y = -pcf; y <= pcf; ++y)
   		{
        	float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
        	shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
			retDiv += 1.0;
    	}
	}

	shadow /= retDiv;
    return shadow;
	}
}

 void main() {
 	vec3 N = normalize(inNormals);
	vec3 V = normalize(ubo.cameraPosition - inWorldPosition);

	vec3 reflectivity = mix(vec3(0.04f), material.albedo, material.metallicness);
	vec3 reflectance  = vec3(0.0f);

	// float bias = max(0.05 * (1.0 - dot(inNormals, inLightPosition.xyz)), 0.001);
	float shadow      = ShadowCalculation(inLightPosition, ubo.pcf2, 0.001);

	reflectance += (1.0f - shadow) * CalculateDirectionalLights(N, V, reflectivity, directionalLight);

	vec3 ambient = ubo.ambient * material.albedo;

	vec3 color = ambient + reflectance;
	color      = color / (color + vec3(1.0f));
	color      = pow(color, vec3(1.0f / 2.2f));

	outFragColor = vec4(color, material.transparency);
}
