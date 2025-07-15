#include "Common.hlsli"


// =============================================================================================== //
// VERTEX SHADER
// =============================================================================================== //
float4 VS_Main(float4 VertexPosition : POSITION) : SV_Position
{
    const float4 wsVertexPosition = mul(VertexPosition, ObjectTransforms[ObjectParameter.TransformIndex]);
    const float4x4 vpMatrix = WorldData.Cameras[ObjectParameter.CameraIndex].ViewProjectionMatrix;
    
    return mul(wsVertexPosition, vpMatrix);
}
