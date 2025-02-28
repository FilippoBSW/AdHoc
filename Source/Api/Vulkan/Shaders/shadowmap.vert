#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive    : enable
//#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 inPosition;

layout (set = 0, binding = 0) uniform LightSpace {
   mat4 uLightSpace;
};

layout(push_constant) uniform Model {
   mat4 uModel;
};

void main() {
   gl_Position = uLightSpace * uModel * vec4(inPosition, 1.0f);
}
