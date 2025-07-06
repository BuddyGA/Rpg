#include "Common.hlsli"


// =============================================================================================== //
// LAYOUTS
// =============================================================================================== //
struct VS_Input
{
    // Slot 0
    float4 Position : POSITION;
    
    // Slot 1
    float4 Normal : NORMAL;
    float4 Tangent : TANGENT;
    
    // Slot 2
    float2 TexCoord : TEXCOORD;
    
    // Slot 3
    float4 BoneWeights0 : BONEWEIGHTS0;
    float4 BoneWeights1 : BONEWEIGHTS1;
    uint4 BoneIndices0 : BONEINDICES0;
    uint4 BoneIndices1 : BONEINDICES1;
    uint BoneCount : BONECOUNT;
};


struct VS_Output
{
    float4 SvPosition : SV_POSITION;
    float4 WorldPosition : WORLD_POSITION;
    float4 WorldNormal : WORLD_NORMAL;
    float4 WorldTangent : WORLD_TANGENT;
    float4 CameraWorldPosition : CAMERA_WORLD_POSITION;
    float2 TexCoord : TEXCOORD;
};



// =============================================================================================== //
// MAIN ENTRY POINT
// =============================================================================================== //
VS_Output VS_Main(VS_Input input)
{
    const RpgShaderConstantCamera camera = WorldData.Cameras[ObjectParameter.CameraIndex];
    const float4x4 worldTransformMatrix = ObjectTransforms[ObjectParameter.TransformIndex];
    
    VS_Output output;
    
    float4x4 boneVertexBlendMatrix = 0;
    
    for (int b = 0; b < input.BoneCount; ++b)
    {
        if (b < 4)
        {
            boneVertexBlendMatrix += mul(input.BoneWeights0[b], Rpg_GetBoneSkinningTransform(ObjectParameter.SkeletonIndex, input.BoneIndices0[b]));
        }
        else
        {
            boneVertexBlendMatrix += mul(input.BoneWeights1[b - 4], Rpg_GetBoneSkinningTransform(ObjectParameter.SkeletonIndex, input.BoneIndices1[b - 4]));
        }
    }
        
    float4 vertexBlendPosition = mul(input.Position, boneVertexBlendMatrix);
    float4 vertexBlendNormal = mul(input.Normal, boneVertexBlendMatrix);
    float4 vertexBlendTangent = mul(input.Tangent, boneVertexBlendMatrix);
    
    output.WorldPosition = mul(vertexBlendPosition, worldTransformMatrix);
    output.WorldNormal = normalize(mul(vertexBlendNormal, worldTransformMatrix));
    output.WorldTangent = normalize(mul(vertexBlendTangent, worldTransformMatrix));
    
    //output.WorldPosition = mul(vertexPosition, worldTransformMatrix).xyz;
    //output.WorldNormal = normalize(mul(vertexNormal, worldTransformMatrix).xyz);
    
    output.SvPosition = mul(output.WorldPosition, camera.ViewProjectionMatrix);
    output.CameraWorldPosition = camera.WorldPosition;
    output.TexCoord = input.TexCoord;
    
    return output;
}
