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