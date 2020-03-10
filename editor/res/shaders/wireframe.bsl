#include "$ENGINE$\BasePass.bslinc"
#include "$ENGINE$\GBufferOutput.bslinc"

shader wireframe
{
	mixin BasePass;
	mixin GBufferOutput;

	raster
	{
		fill = wire;
	};

	code
	{
		cbuffer MaterialParams 
		{
			float4 gWireColor = { 1.0f, 0.0f, 0.0f, 1.0f };
		};

		void fsmain(
			in VStoFS input, 
			out float4 OutSceneColor : SV_Target0,
			out GBufferData OutGBuffer)
		{
			SurfaceData surfaceData;
			surfaceData.albedo = gWireColor;
			surfaceData.worldNormal.xyz = input.tangentToWorldZ;
			surfaceData.roughness = 1.0f;
			surfaceData.metalness = 0.0f;
			surfaceData.mask = gLayer;
			surfaceData.velocity = 0.0f;
			
			OutSceneColor = 0.0f;
			OutGBuffer = encodeGBuffer(surfaceData);
		}	
	};
};