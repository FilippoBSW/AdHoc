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
// #extension GL_EXT_debug_printf : enable
// debugPrintfEXT("currentDepth %f", currentDepth);

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormals;

layout(location = 0) out vec3 outNormals;
layout(location = 1) out vec3 outWorldPosition;
layout(location = 2) out vec4 outLightPosition;

layout(set = 0, binding = 0) uniform UBO  {
	mat4 projView;
};

layout(set = 0, binding = 1) uniform LightSpace {
    mat4 lightSpace;
};

layout(push_constant) uniform Model {
     mat4 model;
};

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	outNormals       = mat3(transpose(inverse(model))) * inNormals;
	outWorldPosition = vec3(model * vec4(inPosition, 1.0f));
	outLightPosition = lightSpace * vec4(outWorldPosition, 1.0f);
	gl_Position      = projView * vec4(outWorldPosition, 1.0f);
}
