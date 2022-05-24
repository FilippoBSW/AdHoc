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

layout (binding = 1) uniform sampler2D color1;
layout (binding = 2) uniform sampler2D color2;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

layout(push_constant) uniform BlurDirection {
    int width;
    int height;
};

void main() {
    vec2 texelSize = vec2(1.0 / width, 1.0 / height);

    const vec2 UPSAMPLE_OFFSETS[4] = vec2[]
    (
        vec2(-1.0, -1.0) * texelSize,
        vec2(-1.0, 1.0) * texelSize,
        vec2(1.0, -1.0) * texelSize,
        vec2(1.0, 1.0) * texelSize
    );

    vec4 uv1 = texture(color2, inUV + UPSAMPLE_OFFSETS[0]);
    vec4 uv2 = texture(color2, inUV + UPSAMPLE_OFFSETS[1]);
    vec4 uv3 = texture(color2, inUV + UPSAMPLE_OFFSETS[2]);
    vec4 uv4 = texture(color2, inUV + UPSAMPLE_OFFSETS[3]);

    vec4 s = uv1 + uv2 + uv3 + uv4;
    s /= 4;

    vec3 res = texture(color1, inUV).rgb + s.rgb;
    outFragColor = vec4(res, 1.0);
}