#include "Common.hlsli"


// =============================================================================================== //
// LAYOUT
// =============================================================================================== //
struct VS_Input
{
    float2 Position : POSITION;
    float4 Color : COLOR;
};


struct VS_Output
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};



// =============================================================================================== //
// FUNCTION
// =============================================================================================== //




// =============================================================================================== //
// MAIN ENTRY POINT
// =============================================================================================== //
VS_Output VS_Main(VS_Input input)
{
    const float2 scaledPosition = float2(input.Position.x * (2.0f / ViewportParameter.Width), input.Position.y * (2.0f / ViewportParameter.Height));
    
    VS_Output output;
    output.Position = float4(float2(scaledPosition.x - 1.0f, 1.0f - scaledPosition.y), 0.0f, 1.0f);
    output.Color = input.Color;
    
    return output;
}
