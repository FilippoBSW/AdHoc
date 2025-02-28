#pragma once
#include <Math/Math.hpp>

namespace adh {
    //************************************************************************
    // @brief	http://wiki.ogre3d.org/-Point+Light+Attenuation
    //
    //************************************************************************
    struct Attenuation {
        alignas(4) float constant;  // Keep to 1.0f
        alignas(4) float linear;    // Multiplied by distance, reduces light in a linear fashion
        alignas(4) float quadratic; // Multiplied by squared distance, gets more and more significant as distance grows.
    };

    enum class AttenuationStrength {
        e1  = 7,
        e2  = 13,
        e3  = 20,
        e4  = 32,
        e5  = 50,
        e6  = 65,
        e7  = 100,
        e8  = 160,
        e9  = 200,
        e10 = 325,
        e11 = 600,
        e12 = 3250,
    };

    struct DirectionalLight {
        alignas(16) Vector3D direction;
        alignas(16) Vector3D color;
        alignas(4) float intensity;
        alignas(4) int castsShadow = false;
        alignas(4) int samplerId   = -1;
    };

    struct PointLight {
        alignas(4) float intensity;
        alignas(16) Vector3D position;
        alignas(16) Vector3D color;
        alignas(16) Attenuation attenuation;
    };

    struct SpotLight {
        alignas(16) Vector3D position;
        alignas(16) Vector3D color;
        alignas(16) Attenuation attenuation;
        alignas(16) Vector3D direction;
        alignas(4) float coneSize;   // from 0.0 to 0.99
        alignas(4) float smoothness; // from 0.1 to however much
        alignas(4) float intensity;
    };

    static void SetLightAttenuation(PointLight* light, AttenuationStrength strenght) {
        switch (strenght) {
        case AttenuationStrength::e1:
            light->attenuation = { 1.0f, 0.7f, 1.8f };
            break;

        case AttenuationStrength::e2:
            light->attenuation = { 1.0f, 0.35f, 0.44f };
            break;

        case AttenuationStrength::e3:
            light->attenuation = { 1.0f, 0.22f, 0.20f };
            break;

        case AttenuationStrength::e4:
            light->attenuation = { 1.0f, 0.14f, 0.07f };
            break;

        case AttenuationStrength::e5:
            light->attenuation = { 1.0f, 0.09f, 0.032f };
            break;

        case AttenuationStrength::e6:
            light->attenuation = { 1.0f, 0.07f, 0.017f };
            break;

        case AttenuationStrength::e7:
            light->attenuation = { 1.0f, 0.045f, 0.0075f };
            break;

        case AttenuationStrength::e8:
            light->attenuation = { 1.0f, 0.027f, 0.0028f };
            break;

        case AttenuationStrength::e9:
            light->attenuation = { 1.0f, 0.022f, 0.0019f };
            break;

        case AttenuationStrength::e10:
            light->attenuation = { 1.0f, 0.014f, 0.0007f };
            break;

        case AttenuationStrength::e11:
            light->attenuation = { 1.0f, 0.007f, 0.0002f };
            break;

        case AttenuationStrength::e12:
            light->attenuation = { 1.0f, 0.0014f, 0.000007f };
            break;
        }
    }

    static void SetLightAttenuation(SpotLight* light, AttenuationStrength strenght) {
        switch (strenght) {
        case AttenuationStrength::e1:
            light->attenuation = { 1.0f, 0.7f, 1.8f };
            break;

        case AttenuationStrength::e2:
            light->attenuation = { 1.0f, 0.35f, 0.44f };
            break;

        case AttenuationStrength::e3:
            light->attenuation = { 1.0f, 0.22f, 0.20f };
            break;

        case AttenuationStrength::e4:
            light->attenuation = { 1.0f, 0.14f, 0.07f };
            break;

        case AttenuationStrength::e5:
            light->attenuation = { 1.0f, 0.09f, 0.032f };
            break;

        case AttenuationStrength::e6:
            light->attenuation = { 1.0f, 0.07f, 0.017f };
            break;

        case AttenuationStrength::e7:
            light->attenuation = { 1.0f, 0.045f, 0.0075f };
            break;

        case AttenuationStrength::e8:
            light->attenuation = { 1.0f, 0.027f, 0.0028f };
            break;

        case AttenuationStrength::e9:
            light->attenuation = { 1.0f, 0.022f, 0.0019f };
            break;

        case AttenuationStrength::e10:
            light->attenuation = { 1.0f, 0.014f, 0.0007f };
            break;

        case AttenuationStrength::e11:
            light->attenuation = { 1.0f, 0.007f, 0.0002f };
            break;

        case AttenuationStrength::e12:
            light->attenuation = { 1.0f, 0.0014f, 0.000007f };
            break;
        }
    }
} // namespace adh
