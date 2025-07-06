#include "Common.hlsli"


// =============================================================================================== //
// LAYOUT
// =============================================================================================== //
struct PS_Input
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR0;
};



// =============================================================================================== //
// MAIN ENTRY POINT
// =============================================================================================== //
float4 PS_Main(PS_Input input) : SV_TARGET
{
#ifdef DEPTH
    const float D = Rpg_GetMaterialParameterTextureColor(MaterialParameter.TextureDescriptorIndex_BaseColor, SamplerPoint, input.TexCoord).r * 2.0f - 1.0f;
    /*
    const float N = 10.0f;
    const float F = 10000.0f;
    const float Ld = ((2.0f * F * N) / (F + N - D * (F - N))) / F;
    */
    
    const float Ld = 0.01f / (1.01f - D);
    
    return float4(Ld, Ld, Ld, 1.0f);
    
#elif FONT
    return float4(input.Color.rgb, Rpg_GetMaterialParameterTextureColor(MaterialParameter.TextureDescriptorIndex_OpacityMask, SamplerPoint, input.TexCoord).r * input.Color.a);
    
#else
    return input.Color * Rpg_GetMaterialParameterTextureColor(MaterialParameter.TextureDescriptorIndex_BaseColor, SamplerPoint, input.TexCoord);
    
#endif // FONT
}
