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

layout (binding = 1) uniform sampler2D samplerColor;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

layout (binding = 0) uniform UBO {
	float blurScale;
	float blurStrength;
} ubo;

layout(push_constant) uniform BlurDirection {
     int blurDirection;
};

// From the OpenGL Super bible
const float weight[] = float[](0.0024499299678342,
								0.0043538453346397,
								0.0073599963704157,
								0.0118349786570722,
								0.0181026699707781,
								0.0263392293891488,
								0.0364543006660986,
								0.0479932050577658,
								0.0601029809166942,
								0.0715974486241365,
								0.0811305381519717,
								0.0874493212267511,
								0.0896631113333857,
								0.0874493212267511,
								0.0811305381519717,
								0.0715974486241365,
								0.0601029809166942,
								0.0479932050577658,
								0.0364543006660986,
								0.0263392293891488,
								0.0181026699707781,
								0.0118349786570722,
								0.0073599963704157,
								0.0043538453346397,
								0.0024499299678342);

void main(){
	vec2 tex_offset = 1.0 / textureSize(samplerColor, 0) * ubo.blurScale;
	vec3 result = texture(samplerColor, inUV).rgb * weight[0];
	for(int i = 1; i < weight.length(); ++i) {
		if (blurDirection == 1) {
			// H
			result += texture(samplerColor, inUV + vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * ubo.blurStrength;
			result += texture(samplerColor, inUV - vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * ubo.blurStrength;
		}
		else {
			// V
			result += texture(samplerColor, inUV + vec2(0.0, tex_offset.y * i)).rgb * weight[i] * ubo.blurStrength;
			result += texture(samplerColor, inUV - vec2(0.0, tex_offset.y * i)).rgb * weight[i] * ubo.blurStrength;
		}
	}
	outFragColor = vec4(result, 1.0);
}