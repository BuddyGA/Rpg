#pragma once


#ifndef RPG_SHADER_HLSL

#include "core/RpgConfig.h"
#include "core/RpgMath.h"


typedef DirectX::XMVECTOR   RpgShaderFloat4;
typedef DirectX::XMMATRIX   RpgShaderMatrix;
typedef DirectX::XMINT4     RpgShaderInt4;


#define RPG_SHADER_CONSTANT_STATIC_ASSERT_LIMIT(type, alignmentSizeBytes, limitSizeBytes)   \
static_assert(sizeof(type) % alignmentSizeBytes == 0, "Shader constant type of <" ## #type ## "> must be multiple of " ## #alignmentSizeBytes ## " bytes!"); \
static_assert(sizeof(type) <= limitSizeBytes, "Shader constant type of <" ## #type ## "> exceeds limit size bytes!");


#else
#include "../../core/RpgConfig.h"

typedef float4      RpgShaderFloat4;
typedef float4x4    RpgShaderMatrix;
typedef int4        RpgShaderInt4;

#define RPG_SHADER_CONSTANT_STATIC_ASSERT_LIMIT(type, alignmentSizeBytes, limitSizeBytes)   

#endif // !RPG_SHADER_HLSL



struct RpgShaderConstantMaterialVectorScalarData
{
    RpgShaderFloat4 Vectors[RPG_MATERIAL_PARAM_VECTOR_COUNT];
    float Scalars[RPG_MATERIAL_PARAM_SCALAR_COUNT];
};
RPG_SHADER_CONSTANT_STATIC_ASSERT_LIMIT(RpgShaderConstantMaterialVectorScalarData, 16, 256)


typedef RpgShaderMatrix     RpgShaderConstantObjectTransform;
typedef RpgShaderMatrix     RpgShaderConstantSkeletonBoneSkinningTransform;



struct RpgShaderConstantCamera
{
    // Used in world renderer and shadow map renderer
    RpgShaderMatrix ViewMatrix;

    // Used in world renderer and shadow map renderer
    RpgShaderMatrix ViewProjectionMatrix;

    // [XYZ]: World position, [W]: 1.0f
    RpgShaderFloat4 WorldPosition;

    // Near clip range
    float NearClipZ;

    // Far clip range
    float FarClipZ;
};


struct RpgShaderConstantLight
{
    // [XYZ]: World position, [W]: 1.0f
    RpgShaderFloat4 Position;

    // [XYZ]: Unit vector, [W]: 0.0f
    RpgShaderFloat4 Direction;

    // [XYZ]: RGB color, [W]: Intensity
    RpgShaderFloat4 ColorIntensity;

    float AttenuationRadius;
    float AttenuationFallOffExp;

    // For spotlight only. Inner cone angle in radian
    float SpotLightInnerConeRadian;

    // For spotlight only. Outer cone angle in radian
    float SpotLightOuterConeRadian;

    // Shadow camera index used in shadow map renderer. (-1) if light does not cast shadow
    int ShadowCameraIndex;

    // Shadow texture descriptor index. (-1) if light does not cast shadow
    int ShadowTextureDescriptorIndex;
};



struct RpgShaderConstantWorldData
{
    // All cameras
    RpgShaderConstantCamera Cameras[RPG_RENDER_MAX_CAMERA];

    // All lights
    RpgShaderConstantLight Lights[RPG_RENDER_MAX_LIGHT];

    // [XYZ]: RGB color, [W]: Strength multiplier
    RpgShaderFloat4 AmbientColorStrength;

    float DeltaTime;
    int CameraCount;
    int DirectionalLightCount;
    int PointLightCount;
    int SpotLightCount;
};
RPG_SHADER_CONSTANT_STATIC_ASSERT_LIMIT(RpgShaderConstantWorldData, 16, RPG_MEMORY_SIZE_KiB(64))



struct RpgShaderConstantViewportParameter
{
    float Width;
    float Height;
};
RPG_SHADER_CONSTANT_STATIC_ASSERT_LIMIT(RpgShaderConstantViewportParameter, 4, 8)



struct RpgShaderConstantMaterialParameter
{
    int TextureDescriptorIndex_BaseColor;
    int TextureDescriptorIndex_Normal;
    int TextureDescriptorIndex_Metalness;
    int TextureDescriptorIndex_Specular;
    int TextureDescriptorIndex_Roughness;
    int TextureDescriptorIndex_AmbOcc;
    int TextureDescriptorIndex_OpacityMask;
    int TextureDescriptorIndex_Custom0;
    int TextureDescriptorIndex_Custom1;
    int TextureDescriptorIndex_Custom2;
    int TextureDescriptorIndex_Custom3;
    int TextureDescriptorIndex_Custom4;
    int TextureDescriptorIndex_Custom5;
    int TextureDescriptorIndex_Custom6;
    int TextureDescriptorIndex_Custom7;
    int VectorScalarValueIndex;
};
RPG_SHADER_CONSTANT_STATIC_ASSERT_LIMIT(RpgShaderConstantMaterialParameter, 4, 64)



struct RpgShaderConstantObjectParameter
{
    int CameraIndex;
    int TransformIndex;
};
RPG_SHADER_CONSTANT_STATIC_ASSERT_LIMIT(RpgShaderConstantObjectParameter, 4, 16)



struct RpgShaderConstantSkinnedObjectParameter
{
    int SkeletonIndex;
    int VertexStart;
    int VertexCount;
    int IndexStart;
    int IndexCount;
    int SkinnedVertexStart;
    int SkinnedIndexStart;
};
