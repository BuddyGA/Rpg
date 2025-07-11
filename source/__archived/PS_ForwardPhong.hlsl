#include "Common.hlsli"


// =============================================================================================== //
// CONSTANTS
// =============================================================================================== //

#define RPG_MATERIAL_PARAM_VECTOR_INDEX_base_color              0
#define RPG_MATERIAL_PARAM_VECTOR_INDEX_specular_color          1

#define RPG_MATERIAL_PARAM_SCALAR_INDEX_specular_intensity      0
#define RPG_MATERIAL_PARAM_SCALAR_INDEX_shininess               1
#define RPG_MATERIAL_PARAM_SCALAR_INDEX_opacity                 2



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

float Rpg_Phong_CalculateLightShadowFactor_Directional(float3 worldPosition, float4x4 shadowCameraViewProjectionMatrix, int shadowTextureDescriptorIndex)
{
    const float4 shadowProjPosition = mul(shadowCameraViewProjectionMatrix, float4(worldPosition, 1.0f));

    float3 shadowProjCoords = (shadowProjPosition.xyz / shadowProjPosition.w);
    shadowProjCoords.x = 0.5f + shadowProjCoords.x * 0.5f;
    shadowProjCoords.y = 0.5f - shadowProjCoords.y * 0.5f;
    
    return DynamicIndexingTexture2Ds[shadowTextureDescriptorIndex].SampleCmpLevelZero(SamplerShadow, shadowProjCoords.xy, shadowProjCoords.z).r;
}


float Rpg_Phong_CalculateLightShadowFactor_OmniDirectional(float3 fragWorldPosition, float3 lightWorldPosition, float shadowCameraFarClipZ, int shadowTextureDescriptorIndex)
{
    const float3 shadowProjCoords = fragWorldPosition - lightWorldPosition;
    return DynamicIndexingTextureCubes[shadowTextureDescriptorIndex].SampleCmpLevelZero(SamplerShadow, shadowProjCoords, length(shadowProjCoords) / shadowCameraFarClipZ).r;
}


float Rpg_Phong_CalculateLightDistanceAttenuation(float3 lightVector, float lightRadius, float lightFallOffExponent)
{
    /*
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
    */
    const float distanceSqr = dot(lightVector, lightVector);
    const float invRadius = 1.0f / lightRadius;
    const float lightRadiusMask = RPG_SQR(saturate(1.0f - RPG_SQR(distanceSqr * invRadius * invRadius)));
    float distanceAttenuation = (1.0f / (distanceSqr + 1.0f)) * lightRadiusMask;
            
    if (lightFallOffExponent > 0.0f)
    {
        const float distanceAttenuationMask = 1.0f - saturate(dot(lightVector * invRadius, lightVector * invRadius));
        distanceAttenuation = pow(distanceAttenuationMask, lightFallOffExponent);
    }
    
    return distanceAttenuation;
}


float3 Rpg_Phong_CalculateLightContributionColor(float3 surfaceNormal, float3 toViewDirection, float3 toLightDirection, float4 lightColorIntensity, float lightAttenuation, float3 diffuseColor, float specularColor, float shininess)
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
        const float2 scaledUV = input.TexCoord * 16.0f;
        const float2 checker = floor(scaledUV) % 2.0f; //fmod(floor(scaledUV), 2.0f);
        diffuseColor = (checker.x + checker.y) % 2.0f < 1.0f ? 0.5f : 0.25f; // fmod(checker.x + checker.y, 2.0f) < 1.0f ? 0.5f : 0.25f;
    }
    
    
    // Normal
    if (MaterialParameter.TextureDescriptorIndex_Normal != -1)
    {
        float3 worldTangent = normalize(input.WorldTangent).xyz;
        worldTangent = normalize(worldTangent - dot(worldTangent, worldNormal) * worldNormal);
    
        const float3 worldBitangent = cross(worldNormal, worldTangent);
        const float3x3 worldTangentMatrix = float3x3(worldTangent, worldBitangent, worldNormal);
        
        worldNormal = Rpg_GetMaterialParameterTextureColor(MaterialParameter.TextureDescriptorIndex_Normal, SamplerMipMapLinear, input.TexCoord).rgb;
        worldNormal = worldNormal * 2.0f - 1.0f;
        worldNormal = normalize(mul(worldTangentMatrix, worldNormal));
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
    
    
    // Directional lights
    for (int d = RPG_RENDER_LIGHT_DIRECTIONAL_INDEX; d < (RPG_RENDER_LIGHT_DIRECTIONAL_INDEX + WorldData.DirectionalLightCount); ++d)
    {
        const RpgShaderConstantLight directionalLight = WorldData.Lights[d];
        lightContribColor += Rpg_Phong_CalculateLightContributionColor(worldNormal, toViewDir, normalize(-directionalLight.Direction.xyz), directionalLight.ColorIntensity, 1.0f, diffuseColor, specularColor, shininess);
    }
    
    
    // Point lights
    for (int p = RPG_RENDER_LIGHT_POINT_INDEX; p < (RPG_RENDER_LIGHT_POINT_INDEX + WorldData.PointLightCount); ++p)
    {
        const RpgShaderConstantLight pointLight = WorldData.Lights[p];
        const float3 lightPosition = pointLight.Position.xyz;
        const float3 lightVector = lightPosition - worldPosition;
        const float distanceToLight = length(lightVector);
        
        if (distanceToLight <= pointLight.AttenuationRadius)
        {
            float shadowFactor = 1.0f;
                
            // if has shadow camera and shadow texture
            if (pointLight.ShadowCameraIndex != -1 && pointLight.ShadowTextureDescriptorIndex != -1)
            {
                const RpgShaderConstantCamera cameraShadow = WorldData.Cameras[NonUniformResourceIndex(pointLight.ShadowCameraIndex)];
                shadowFactor = Rpg_Phong_CalculateLightShadowFactor_OmniDirectional(worldPosition, lightPosition, cameraShadow.FarClipZ, pointLight.ShadowTextureDescriptorIndex);
            }
            
            // final point light attenuation factor
            const float attenuationFactor = Rpg_Phong_CalculateLightDistanceAttenuation(lightVector, pointLight.AttenuationRadius, pointLight.AttenuationFallOffExp);
            
            lightContribColor += Rpg_Phong_CalculateLightContributionColor(worldNormal, toViewDir, normalize(lightVector), pointLight.ColorIntensity, attenuationFactor, diffuseColor, specularColor, shininess) * shadowFactor;
        }
    }
    
    
    // Spot lights
    for (int s = RPG_RENDER_LIGHT_SPOT_INDEX; s < (RPG_RENDER_LIGHT_SPOT_INDEX + WorldData.SpotLightCount); ++s)
    {
        const RpgShaderConstantLight spotLight = WorldData.Lights[s];
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
                    const RpgShaderConstantCamera cameraShadow = WorldData.Cameras[NonUniformResourceIndex(spotLight.ShadowCameraIndex)];
                    shadowFactor = Rpg_Phong_CalculateLightShadowFactor_Directional(worldPosition, cameraShadow.ViewProjectionMatrix, spotLight.ShadowTextureDescriptorIndex);
                }
                
                // distance attenuation
                const float distanceAttenuation = Rpg_Phong_CalculateLightDistanceAttenuation(lightVector, spotLight.AttenuationRadius, spotLight.AttenuationFallOffExp);
                
                // spot attenuation
                const float cosInnerCutOff = cos(spotLight.SpotLightInnerConeRadian);
                const float spotAttenuationMask = saturate((cosLS - cosOuterCutOff) / (cosInnerCutOff - cosOuterCutOff));
                const float spotAttenuation = RPG_SQR(spotAttenuationMask);

                // final spot light attenuation factor
                const float attenuationFactor = distanceAttenuation * spotAttenuation;
                
                lightContribColor += Rpg_Phong_CalculateLightContributionColor(worldNormal, toViewDir, normalizedLightVector, spotLight.ColorIntensity, attenuationFactor, diffuseColor, specularColor, shininess) * shadowFactor;
            }
        }
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
