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
    output.WorldPosition = mul(input.Position, worldTransformMatrix);
    output.WorldNormal = normalize(mul(input.Normal, worldTransformMatrix));
    output.WorldTangent = normalize(mul(input.Tangent, worldTransformMatrix));
    output.SvPosition = mul(output.WorldPosition, camera.ViewProjectionMatrix);
    output.CameraWorldPosition = camera.WorldPosition;
    output.TexCoord = input.TexCoord;
    
    return output;
}
