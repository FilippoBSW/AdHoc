#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive    : enable

//************************************************************************
// @brief	DistributionGGX.
//
//************************************************************************
float DistributionGGX(float NdotH, float roughness) {
	float alpha  = roughness * roughness;
	float alpha2 = alpha * alpha;

	float denom = NdotH * NdotH * (alpha2 - 1.0f) + 1.0f;
	denom = PI * denom * denom;

	return alpha2 / max(denom, 0.0000001f);
}

//************************************************************************
// @brief	GeometrySmith.
//
//************************************************************************
float GeometrySmith(float NdotV, float NdotL, float roughness) {
	float r = roughness + 1.0f;
	float k = (r * r) / 8.0f;

	float ggx1 = NdotV / (NdotV * (1.0f - k) + k);
	float ggx2 = NdotL / (NdotL * (1.0f - k) + k);

	return ggx1 * ggx2;
}

//************************************************************************
// @brief	FresnelSchlick.
//
//************************************************************************
vec3 FresnelSchlick(float NdotV, vec3 baseReflectivity) {
	return baseReflectivity + (1.0f - baseReflectivity) * pow(1.0f - NdotV, 5.0f);
}
