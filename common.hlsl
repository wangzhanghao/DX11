


cbuffer WorldBuffer : register(b0)
{
	matrix World;
}
cbuffer ViewBuffer : register(b1)
{
	matrix View;
}
cbuffer ProjectionBuffer : register(b2)
{
	matrix Projection;
}




struct MATERIAL
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Emission;
	float Shininess;
	float3 Dummy;
};

cbuffer MaterialBuffer : register(b3)
{
	MATERIAL Material;
}

struct LIGHT
{
	bool Enable;
    int frameCount;
	bool2 Dummy;
	float4 Direction;
	float4 Diffuse;
	float4 Ambient;
};

cbuffer LightBuffer : register(b4)
{
	LIGHT Light;
}

struct CAMERA
{
	float4 Position;
};
cbuffer CameraBuffer : register(b5)
{
	CAMERA Camera;
}

struct ATMOSPHERE
{
    float _InnerRadius;
    float _OuterRadius;
    float _Kr;
    float _Km;
    float fSamples;
    float fScaleDepth;
    float2 Dummy;
};
cbuffer AtmosphereBuffer : register(b6)
{
    ATMOSPHERE Atmos;
}

struct Cloud
{
    float speed;
    float3 windDir;
    float matchingstep;
    int matchingcount;
    float2 dummy;
};
cbuffer CloudBuffer : register(b7)
{
    Cloud cloud;
}


struct VS_IN
{
	float4 Position		: POSITION0;
	float4 Normal		: NORMAL0;
	float4 Diffuse		: COLOR0;
	float2 TexCoord		: TEXCOORD0;
};


struct PS_IN
{
	float4 Position		: SV_POSITION;
    float4 WorldPosition : POSITION1;
    float4 WorldPos : POSITION2;
	float4 Diffuse		: COLOR0;
	float2 TexCoord		: TEXCOORD0;
    float iTime:TIME0;
};

#define bottom 0 
#define top 8
#define width 100
#define ScreenHeight 720
#define ScreenWidth 1280
#define baseBright  float3(1.26,1.25,1.29)    
#define baseDark    float3(0.31,0.31,0.32)    
#define lightBright float3(1.29, 1.17, 1.05)  
#define lightDark   float3(0.7,0.75,0.8)    
#define PI 3.14159265359  


