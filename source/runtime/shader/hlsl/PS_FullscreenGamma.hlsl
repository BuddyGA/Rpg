#include "Common.hlsli"


#define MATERIAL_PARAM_SCALAR_INDEX_gamma   0


// =============================================================================================== //
// LAYOUT
// =============================================================================================== //
struct PS_Input
{
    float4 SvPosition : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR0;
};



// =============================================================================================== //
// MAIN ENTRY POINT
// =============================================================================================== //
float4 PS_Main(PS_Input input) : SV_TARGET
{
    float4 linearColor = Rpg_GetMaterialParameterTextureColor(MaterialParameter.TextureDescriptorIndex_BaseColor, SamplerPoint, input.TexCoord);
    float gamma = Rpg_GetMaterialParameterScalarValue(MATERIAL_PARAM_SCALAR_INDEX_gamma);
    
    return float4(pow(linearColor.rgb, 1.0f / gamma), linearColor.a);
    //return linearColor;
}
