#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive    : enable

layout (binding = 1) uniform sampler2D samplerColor;

layout(push_constant) uniform Threshold {
     float threshold;
};

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

void main() {
    vec3 result      = texture(samplerColor, inUV).rgb;
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));

     outFragColor = vec4(result * brightness * threshold, 1.0);

    // if(brightness > threshold){
    //     outFragColor = vec4(result, 1.0);
    // } else {
    //      outFragColor = vec4(0.0, 0.0, 0.0, 1.0);
    // }
}