#include "Common.hlsli"


// =============================================================================================== //
// LAYOUTS
// =============================================================================================== //
struct VS_Input
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
};


struct VS_Output
{
    float4 SvPosition : SV_POSITION;
    float4 Color : COLOR0;
};



// =============================================================================================== //
// MAIN ENTRY POINT
// =============================================================================================== //
VS_Output VS_Main(VS_Input input)
{
    const float4x4 viewProjectionMatrix = WorldData.Cameras[ObjectParameter.CameraIndex].ViewProjectionMatrix;
    
    VS_Output output;
    output.SvPosition = mul(input.Position, viewProjectionMatrix);
    output.Color = input.Color;
    
    return output;
}
