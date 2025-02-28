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