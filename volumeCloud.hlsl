
Texture3D   g_VolumeTex;
 
cbuffer  cbPerObject
{
	row_major	float4x4 	g_worldViewProj : WORLDVIEWPROJECTION;
}
 
cbuffer  cbPerFrame
{
	float           g_ftime : TIME;
}
 
 
RasterizerState DisableCulling
{
	CullMode = NONE;
};
 
 
SamplerState samTriLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
 
struct  VertexIn
{
	float3 Pos:POSITION;
};
 
struct  VertexOut
{
	float4 PosH           :SV_POSITION;
	float3 PosW           :POSITION;
};
 
VertexOut VS(VertexIn In)
{
	VertexOut  Out = (VertexOut)0;
	Out.PosW = In.Pos;
	Out.PosH = mul(float4(In.Pos, 1.0f), g_worldViewProj);
	return Out;
 
}
 
float4  PS(VertexOut In) :SV_Target
{
	float4 sky = float4(0.0f, 0.0f, 1.0f, 0.8f);
	float4 cloud = float4(1.0f, 1.0f, 1.0f, 1.0f);
 
	In.PosW.xy += g_ftime*3.7;
	In.PosW.z += g_ftime*4.8;
 
	float noisy = g_VolumeTex.Sample(samTriLinear, In.PosW.xyz/100.0).r;
	float lrp = noisy * 3 - 1.0f;
	
	return lerp(cloud, sky, saturate(lrp));
}
 
technique11 Draw
{
	Pass p0
	{
	SetVertexShader(CompileShader(vs_5_0, VS()));
	SetPixelShader(CompileShader(ps_5_0, PS()));
	SetRasterizerState(DisableCulling);
         }
 
}