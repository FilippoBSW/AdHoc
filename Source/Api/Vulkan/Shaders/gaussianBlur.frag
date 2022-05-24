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

layout (binding = 1) uniform sampler2D image;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

layout (binding = 0) uniform UBO {
	float blurScale;
	float blurStrength;
};

layout(push_constant) uniform BlurDirection {
     int horizontalBlur;
         int width;
    int height;
};

const float weight[] = float[] (0.227027,
                                0.1945946,
                                0.1216216,
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

void main()
{             
    vec2 tex_offset = 1.0 / textureSize(image, 0) * blurStrength;
    vec3 result = texture(image, inUV).rgb * weight[0];

    if(horizontalBlur == 0)
    {
        vec2 texelSize = vec2(1.0 / width, 1.0 / height);

        const vec2 DOWNSAMPLE_OFFSETS[4] = vec2[]
        (
        vec2(-0.5, -0.5) * texelSize,
        vec2(-0.5, 0.5) * texelSize,
        vec2(0.5, -0.5) * texelSize,
        vec2(0.5, 0.5) * texelSize
        );

        for(int i = 1; i < weight.length(); ++i)
        {
            {
            vec3 uv1 = texture(image, inUV + DOWNSAMPLE_OFFSETS[0] + vec2(tex_offset.x * i * 2, 0.0)).rgb;
            vec3 uv2 = texture(image, inUV + DOWNSAMPLE_OFFSETS[1] + vec2(tex_offset.x * i * 2, 0.0)).rgb;
            vec3 uv3 = texture(image, inUV + DOWNSAMPLE_OFFSETS[2] + vec2(tex_offset.x * i * 2, 0.0)).rgb;
            vec3 uv4 = texture(image, inUV + DOWNSAMPLE_OFFSETS[3] + vec2(tex_offset.x * i * 2, 0.0)).rgb;

            vec3 s = uv1 + uv2 + uv3 + uv4;
            s /= 4;

            result +=  s * weight[i] * blurScale;
            }

            {
            vec3 uv1 = texture(image, inUV + DOWNSAMPLE_OFFSETS[0] - vec2(tex_offset.x * i * 2, 0.0)).rgb;
            vec3 uv2 = texture(image, inUV + DOWNSAMPLE_OFFSETS[1] - vec2(tex_offset.x * i * 2, 0.0)).rgb;
            vec3 uv3 = texture(image, inUV + DOWNSAMPLE_OFFSETS[2] - vec2(tex_offset.x * i * 2, 0.0)).rgb;
            vec3 uv4 = texture(image, inUV + DOWNSAMPLE_OFFSETS[3] - vec2(tex_offset.x * i * 2, 0.0)).rgb;

            vec3 s = uv1 + uv2 + uv3 + uv4;
            s /= 4;

            result +=  s * weight[i] * blurScale;
            }

            // result += texture(image, inUV + vec2(tex_offset.x * i * 2, 0.0)).rgb * weight[i] * blurScale;
            // result += texture(image, inUV - vec2(tex_offset.x * i * 2, 0.0)).rgb * weight[i] * blurScale;
        }

		outFragColor = vec4(result, 1.0);
    }
    else if(horizontalBlur == 1)
    {
        for(int i = 1; i < weight.length(); ++i)
        {

            result += texture(image, inUV + vec2(0.0, tex_offset.y * i)).rgb * weight[i] * blurScale;
            result += texture(image, inUV - vec2(0.0, tex_offset.y * i)).rgb * weight[i] * blurScale;
        }
		outFragColor = vec4(result, 1.0);
    }
     else{
        outFragColor = vec4(texture(image, inUV).rgb, 1.0);
     }
}