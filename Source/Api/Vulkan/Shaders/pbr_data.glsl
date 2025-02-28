#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive    : enable

//************************************************************************
// @brief	Constants.
//
//************************************************************************
const float PI = 3.14159265359;

//************************************************************************
// @brief	Data structures.
//
//************************************************************************
struct Attenuation {
	float constant;
	float linear;
	float quadratic;
};

struct Material {
	float roughness;
	float metallicness;
	float transparency;
	int hasTexture;
	vec3  albedo;
};

struct DirectionalLight {
	vec3  direction;
	vec3  color;
	float intensity;
	int   castsShadow;
	int   samplerId;
};

struct PointLight {
	float       intensity;
	vec3        position;
	vec3        color;
	Attenuation attenuation;
};

struct SpotLight {
	vec3        position;
	vec3        color;
	Attenuation attenuation;

	vec3  direction;
	float coneSize;		 // Phi
	float smoothness;	 // Gamma
	float intensity;
};
