#include "Common.hlsli"


// =============================================================================================== //
// LAYOUT
// =============================================================================================== //
struct GS_Input
{
    float4 SvPosition : SV_Position;
};


struct GS_Output
{
    float4 SvPosition : SV_Position;
    uint RenderTargetIndex : SV_RenderTargetArrayIndex;
};



// =============================================================================================== //
// MAIN ENTRY POINT
// =============================================================================================== //
[maxvertexcount(18)] 
void GS_Main(triangle GS_Input inputs[3], inout TriangleStream<GS_Output> stream)
{
	for (uint rtIndex = 0; rtIndex < 6; ++rtIndex)
	{
        const RpgShaderConstantCamera camera = WorldData.Cameras[ObjectParameter.CameraIndex + rtIndex];
		
		for (int vtxIndex = 0; vtxIndex < 3; ++vtxIndex)
		{
            const float4 worldPosition = inputs[vtxIndex].SvPosition;
			
			GS_Output output;
            output.SvPosition = mul(camera.ViewProjectionMatrix, worldPosition);
			
            const float3 viewPosition = mul(camera.ViewMatrix, worldPosition).xyz;
            output.SvPosition.z = length(viewPosition) / camera.FarClipZ;
            output.RenderTargetIndex = rtIndex;
			
			stream.Append(output);
		}
		
		stream.RestartStrip();
	}
}
