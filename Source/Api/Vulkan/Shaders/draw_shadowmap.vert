#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive    : enable
// #extension GL_EXT_debug_printf : enable

layout (location = 0) out vec2 outUV;

void main()
{
	outUV       = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0f, 1.0f);
}
