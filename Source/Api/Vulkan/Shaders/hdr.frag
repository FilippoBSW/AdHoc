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

layout (binding = 1) uniform sampler2D hdrBuffer;
layout (binding = 2) uniform sampler2D bloomBuffer;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

layout(push_constant) uniform Intensity {
     float intensity;
     float exposure;
};

void main() {
	const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, inUV).rgb;
    vec3 bloomColor = texture(bloomBuffer, inUV).rgb;
    vec3 temp = hdrColor + (bloomColor * intensity);    
    vec3 result = vec3(1.0) - exp(-temp * exposure);       
    result = pow(result, vec3(1.0 / gamma));
    outFragColor = vec4(result, 1.0);
}