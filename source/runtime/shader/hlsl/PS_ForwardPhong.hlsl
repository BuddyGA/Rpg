#include "Common.hlsli"


// =============================================================================================== //
// CONSTANTS
// =============================================================================================== //

#define RPG_MATERIAL_PARAM_VECTOR_INDEX_base_color              0
#define RPG_MATERIAL_PARAM_VECTOR_INDEX_specular_color          1

#define RPG_MATERIAL_PARAM_SCALAR_INDEX_shininess               0
#define RPG_MATERIAL_PARAM_SCALAR_INDEX_opacity                 1



// =============================================================================================== //
// LAYOUTS
// =============================================================================================== //

struct PS_Input
{
    float4 SvPosition : SV_POSITION;
    float4 WorldPosition : WORLD_POSITION;
    float4 WorldNormal : WORLD_NORMAL;
    float4 WorldTangent : WORLD_TANGENT;
    float4 CameraWorldPosition : CAMERA_WORLD_POSITION;
    float2 TexCoord : TEXCOORD;
};



// =============================================================================================== //
// FUNCTIONS
// =============================================================================================== //

float Rpg_PhongLightShadowFactor_Directional(float3 worldPosition, RpgShaderConstantLight light, RpgShaderConstantCamera shadowCamera)
{
    const float4 lsFragPosition = mul(float4(worldPosition, 1.0f), shadowCamera.ViewProjectionMatrix);

    float3 shadowProjCoords = (lsFragPosition.xyz / lsFragPosition.w);
    shadowProjCoords.x = 0.5f + shadowProjCoords.x * 0.5f;
    shadowProjCoords.y = 0.5f + shadowProjCoords.y * 0.5f;
    
    return DynamicIndexingTexture2Ds[light.ShadowTextureDescriptorIndex].SampleCmpLevelZero(SamplerShadow, shadowProjCoords.xy, lsFragPosition.z);
}


float Rpg_PhongLightShadowFactor_OmniDirectional(float3 worldPosition, RpgShaderConstantLight light, RpgShaderConstantCamera shadowCamera)
{
    const float3 shadowProjCoords = worldPosition - light.Position.xyz;
    const float depthValue = length(shadowProjCoords) / shadowCamera.FarClipZ;
   
    return DynamicIndexingTextureCubes[light.ShadowTextureDescriptorIndex].SampleCmpLevelZero(SamplerShadow, shadowProjCoords, depthValue);
}


float Rpg_PhongLightDistanceAttenuation(float3 lightVector, float lightRadius, float lightFallOffExponent)
{
    const float distanceSqr = dot(lightVector, lightVector);
    const float invRadius = 1.0f / lightRadius;
    const float normalizedDistanceSqr = distanceSqr * invRadius * invRadius;
    const float edgeFalloff = RPG_SQR(saturate(1.0f - RPG_SQR(normalizedDistanceSqr)));
    float distanceAttenuation = (1.0f / (distanceSqr + 1.0f)) * edgeFalloff;
            
    if (lightFallOffExponent > 0.0f)
    {
        const float distanceAttenuationMask = 1.0f - saturate(normalizedDistanceSqr);
        distanceAttenuation = pow(distanceAttenuationMask, lightFallOffExponent);
    }
    
    return distanceAttenuation;
}


float3 Rpg_PhongLightContributionColor(float3 surfaceNormal, float3 toViewDirection, float3 toLightDirection, float4 lightColorIntensity, float lightAttenuation, float3 diffuseColor, float specularColor, float shininess)
{
    const float3 baseLightColor = (lightColorIntensity.rgb * lightColorIntensity.a);
    
    const float diffuseFactor = saturate(dot(surfaceNormal, toLightDirection));
    float3 lightContributionColor = baseLightColor * diffuseColor * diffuseFactor * lightAttenuation;

    // Blinn-Phong
    const float3 halfReflectDirection = normalize(toViewDirection + toLightDirection);
    
    const float specularFactor = pow(saturate(dot(surfaceNormal, halfReflectDirection)), shininess);
    lightContributionColor += baseLightColor * specularColor * specularFactor * lightAttenuation;
    
    return lightContributionColor;
}



// =============================================================================================== //
// MAIN ENTRY POINT
// =============================================================================================== //

float4 PS_Main(PS_Input input) : SV_TARGET
{
    float3 worldPosition = input.WorldPosition.xyz;
    float3 worldNormal = normalize(input.WorldNormal).xyz;
    const float3 toViewDir = normalize(input.CameraWorldPosition.xyz - worldPosition);
    
    
    // Default values
    float specularColor = 0.5f;
    float shininess = 32.0f;
    float specularStrength = 1.0f;
    
    
    // Diffuse
    float3 diffuseColor = Rpg_GetMaterialParameterVectorValue(RPG_MATERIAL_PARAM_VECTOR_INDEX_base_color).rgb;
    if (MaterialParameter.TextureDescriptorIndex_BaseColor != -1)
    {
        diffuseColor = Rpg_GetMaterialParameterTextureColor(MaterialParameter.TextureDescriptorIndex_BaseColor, SamplerMipMapLinear, input.TexCoord).rgb;
    }
    else
    {
        const float2 scaledUV = input.TexCoord;
        const float2 checker = floor(scaledUV) % 2.0f; //fmod(floor(scaledUV), 2.0f);
        diffuseColor = (checker.x + checker.y) % 2.0f < 1.0f ? 0.5f : 0.25f; // fmod(checker.x + checker.y, 2.0f) < 1.0f ? 0.5f : 0.25f;
    }
    
    
    // Normal
    if (MaterialParameter.TextureDescriptorIndex_Normal != -1)
    {
        float3 worldTangent = normalize(input.WorldTangent).xyz;
        worldTangent = normalize(worldTangent - dot(worldTangent, worldNormal) * worldNormal);
    
        const float3 worldBitangent = normalize(cross(worldNormal, worldTangent));
        const float3x3 worldTangentMatrix = float3x3(worldTangent, worldBitangent, worldNormal);
        
        worldNormal = Rpg_GetMaterialParameterTextureColor(MaterialParameter.TextureDescriptorIndex_Normal, SamplerMipMapLinear, input.TexCoord).rgb;
        //worldNormal = worldNormal * 2.0f - 1.0f; // Using BC5_SNORM
        worldNormal = normalize(mul(worldNormal, worldTangentMatrix));
    }
    
    
    // Specular
    specularColor = Rpg_GetMaterialParameterVectorValue(RPG_MATERIAL_PARAM_VECTOR_INDEX_specular_color).r;
    if (MaterialParameter.TextureDescriptorIndex_Specular != -1)
    {
        specularColor = Rpg_GetMaterialParameterTextureColor(MaterialParameter.TextureDescriptorIndex_Specular, SamplerMipMapLinear, input.TexCoord).r;
    }
    
    shininess = Rpg_GetMaterialParameterScalarValue(RPG_MATERIAL_PARAM_SCALAR_INDEX_shininess);
    
    
    // Ambient light
    const float3 ambientColor = (diffuseColor * WorldData.AmbientColorStrength.rgb * WorldData.AmbientColorStrength.a);
    
    
    float3 lightContribColor = 0.0f;

    
    // Point lights
    for (int pl = RPG_RENDER_LIGHT_POINT_INDEX; pl < (RPG_RENDER_LIGHT_POINT_INDEX + WorldData.PointLightCount); ++pl)
    {
        const RpgShaderConstantLight pointLight = WorldData.Lights[pl];
        const float3 lightPosition = pointLight.Position.xyz;
        const float3 lightVector = lightPosition - worldPosition;
        const float distanceToLight = length(lightVector);
        
        if (distanceToLight <= pointLight.AttenuationRadius)
        {
            float shadowFactor = 1.0f;
                
            // if has shadow camera and shadow texture
            if (pointLight.ShadowCameraIndex != -1 && pointLight.ShadowTextureDescriptorIndex != -1)
            {
                const RpgShaderConstantCamera shadowCamera = WorldData.Cameras[pointLight.ShadowCameraIndex];
                shadowFactor = Rpg_PhongLightShadowFactor_OmniDirectional(worldPosition, pointLight, shadowCamera);
            }
            
            // final point light attenuation factor
            float attenuationFactor = Rpg_PhongLightDistanceAttenuation(lightVector, pointLight.AttenuationRadius, pointLight.AttenuationFallOffExp);
            //attenuationFactor = 1.0f / (0.0f + 0.0f * (distanceToLight / 100.0f) + 0.06f * RPG_SQR(distanceToLight / 100.0f));
            lightContribColor += Rpg_PhongLightContributionColor(worldNormal, toViewDir, normalize(lightVector), pointLight.ColorIntensity, attenuationFactor, diffuseColor, specularColor, shininess) * shadowFactor;
        }
    }
    
    
    // Spot lights
    for (int sl = RPG_RENDER_LIGHT_SPOT_INDEX; sl < (RPG_RENDER_LIGHT_SPOT_INDEX + WorldData.SpotLightCount); ++sl)
    {
        const RpgShaderConstantLight spotLight = WorldData.Lights[sl];
        const float3 lightVector = spotLight.Position.xyz - worldPosition;
        const float distanceToLight = length(lightVector);
        
        if (distanceToLight <= spotLight.AttenuationRadius)
        {
            const float3 normalizedLightVector = normalize(lightVector);
            const float cosLS = dot(normalizedLightVector, normalize(-spotLight.Direction.xyz));
            const float cosOuterCutOff = cos(spotLight.SpotLightOuterConeRadian);
            
            if (cosLS > cosOuterCutOff)
            {
                float shadowFactor = 1.0f;
                
                // if has shadow camera and shadow texture
                if (spotLight.ShadowCameraIndex != -1 && spotLight.ShadowTextureDescriptorIndex != -1)
                {
                    const RpgShaderConstantCamera shadowCamera = WorldData.Cameras[spotLight.ShadowCameraIndex];
                    shadowFactor = Rpg_PhongLightShadowFactor_Directional(worldPosition, spotLight, shadowCamera);
                }
                
                // distance attenuation
                const float distanceAttenuation = Rpg_PhongLightDistanceAttenuation(lightVector, spotLight.AttenuationRadius, spotLight.AttenuationFallOffExp);
                
                // spot attenuation
                const float cosInnerCutOff = cos(spotLight.SpotLightInnerConeRadian);
                const float spotAttenuationMask = saturate((cosLS - cosOuterCutOff) / (cosInnerCutOff - cosOuterCutOff));
                const float spotAttenuation = RPG_SQR(spotAttenuationMask);

                // final spot light attenuation factor
                const float attenuationFactor = distanceAttenuation * spotAttenuation;
                
                lightContribColor += Rpg_PhongLightContributionColor(worldNormal, toViewDir, normalizedLightVector, spotLight.ColorIntensity, attenuationFactor, diffuseColor, specularColor, shininess) * shadowFactor;
            }
        }
    }
    
    
    // Directional lights
    for (int dl = RPG_RENDER_LIGHT_DIRECTIONAL_INDEX; dl < (RPG_RENDER_LIGHT_DIRECTIONAL_INDEX + WorldData.DirectionalLightCount); ++dl)
    {
        const RpgShaderConstantLight directionalLight = WorldData.Lights[dl];
        lightContribColor += Rpg_PhongLightContributionColor(worldNormal, toViewDir, normalize(-directionalLight.Direction.xyz), directionalLight.ColorIntensity, 1.0f, diffuseColor, specularColor, shininess);
    }
    
    
    const float3 finalColor = ambientColor + lightContribColor;
    float finalAlpha = 1.0f;
    
#ifdef MASK
    if (MaterialParameters.TextureDescriptorIndex_OpacityMask != -1)
    {
        finalAlpha = Rpg_GetMaterialParameterTextureColor(MaterialParameter.TextureDescriptorIndex_OpacityMask, input.TexCoord).r;
    }
#endif // MASK
    
    return float4(finalColor, finalAlpha);
}
