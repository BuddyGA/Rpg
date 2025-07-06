#include "Common.hlsli"


// =============================================================================================== //
// LAYOUTS
// =============================================================================================== //
struct VS_Input
{
    float4 Position : POSITION0;
};


struct VS_Output
{
    float4 SvPosition : SV_Position;
};



// =============================================================================================== //
// MAIN ENTRY POINT
// =============================================================================================== //
VS_Output VS_Main(VS_Input input)
{
    const float4x4 worldTransformMatrix = ObjectTransforms[ObjectParameter.TransformIndex];
    const float4 vertexWorldPosition = mul(input.Position, worldTransformMatrix);
    VS_Output output;
    
#ifdef CUBE
    output.SvPosition = vertexWorldPosition;
    
#else
    const float4x4 viewProjectionMatrix = WorldData.Cameras[ObjectParameter.CameraIndex].ViewProjectionMatrix;
    output.SvPosition = mul(vertexWorldPosition, viewProjectionMatrix);
#endif // CUBE
    
    return output;
}
