#include "Common.hlsli"


// =============================================================================================== //
// VERTEX SHADER
// =============================================================================================== //
float4 VS_Main(float4 VertexPosition: POSITION) : SV_Position
{
	return mul(VertexPosition, ObjectTransforms[ObjectParameter.TransformIndex]);
}



// =============================================================================================== //
// GEOMETRY SHADER
// =============================================================================================== //
struct GeometryShaderOutput
{
	float4 SvPosition: SV_Position;
    uint RenderTargetIndex : SV_RenderTargetArrayIndex;
    //nointerpolation float3 WsFragPosition : POSITION0;
    //nointerpolation float3 WsLightPosition : POSITION1;
    //nointerpolation float FarClipZ : CLIP;
};



[maxvertexcount(18)] 
void GS_Main(triangle float4 ws_VertexPositions[3]: SV_Position, inout TriangleStream<GeometryShaderOutput> stream)
{
	for (uint rtIndex = 0; rtIndex < 6; ++rtIndex)
	{
        const RpgShaderConstantCamera camera = WorldData.Cameras[ObjectParameter.CameraIndex + rtIndex];
		
		for (int vtxIndex = 0; vtxIndex < 3; ++vtxIndex)
		{
            const float4 worldPosition = ws_VertexPositions[vtxIndex];
			
			GeometryShaderOutput output;
            output.SvPosition = mul(worldPosition, camera.ViewProjectionMatrix);
            output.SvPosition.z = length(worldPosition.xyz - camera.WorldPosition.xyz) * output.SvPosition.w / camera.FarClipZ;
            output.RenderTargetIndex = rtIndex;
            //output.WsFragPosition = worldPosition.xyz;
            //output.WsLightPosition = camera.WorldPosition.xyz;
            //output.FarClipZ = camera.FarClipZ;
			
			stream.Append(output);
		}
		
		stream.RestartStrip();
	}
}



// =============================================================================================== //
// PIXEL SHADER
// =============================================================================================== //
/*
typedef GeometryShaderOutput PixelShaderInput;

float PS_Main(PixelShaderInput input) : SV_Depth
{
    return length(input.WsFragPosition - input.WsLightPosition) / input.FarClipZ;
}
*/

