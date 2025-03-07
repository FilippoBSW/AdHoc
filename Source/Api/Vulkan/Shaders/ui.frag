#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive    : enable

layout (set = 0, binding = 0) uniform sampler2D fontSampler;

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec4 inColor;

layout (location = 0) out vec4 outColor;

void main() {
	outColor = inColor * texture(fontSampler, inUV);
}
