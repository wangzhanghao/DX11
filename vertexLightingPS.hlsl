
#include "common.hlsl"
#include "PerlinNoise.hlsl"

Texture2D g_Texture : register(t0);
//Texture2D g_PerlinNoise : register(t1);
SamplerState g_SamplerState : register(s0);

#define bottom 0 
#define top 5
#define width 100
#define ScreenHeight 720
#define ScreenWidth 1280
#define baseBright  float3(1.26,1.25,1.29)    
#define baseDark    float3(0.31,0.31,0.32)    
#define lightBright float3(1.29, 1.17, 1.05)  
#define lightDark   float3(0.7,0.75,0.8)      

#define DELTA 0.001
#define PI 3.14159265359

float4 RayleighSct = float4(5.8f, 13.5f, 33.1f, 0.0f) * 0.000001f;

float _AtmosphereHeight = 50.0f;
float _PlanetRadius = 300.0f;//6*_AtmosphereHeight
float2 _DensityScaleHeight = float2(7994.0f, 1200.0f);

float3 _ScatteringR=float3(5.8f, 13.5f, 33.1f) * 0.000001f;
float3 _ScatteringM = float3(2.0f, 2.0f, 2.0f) * 0.00001f;
float3 _ExtinctionR = float3(5.8f, 13.5f, 33.1f) * 0.000001f;
float3 _ExtinctionM = float3(2.0f, 2.0f, 2.0f) * 0.00001f;

float4 _IncomingLight=float4(4, 4, 4, 4);
float _MieG = 0.76;

float _SunIntensity;
float _DistanceScale=1;

static float3 _LightDir = Light.Direction;
struct Ray
{
	float3 dir;
	float3 pos;
};

float2 RaySphereIntersection(float3 rayOrigin, float3 rayDir, float3 sphereCenter, float sphereRadius)
{
    rayOrigin -= sphereCenter;
    float a = dot(rayDir, rayDir);
    float b = 2.0 * dot(rayOrigin, rayDir);
    float c = dot(rayOrigin, rayOrigin) - (sphereRadius * sphereRadius);
    float d = max(0.0, b * b - 4 * a * c);
    d = sqrt(d);
    return float2(-b - d, -b + d) / (2 * a);
    
}

bool lightSampleing(
	float3 position, // Current point within the atmospheric sphere
	float3 lightDir, // Direction towards the sun
	out float2 opticalDepthCP)
{
    opticalDepthCP = 0;


    float3 rayStart = position;
    float3 rayDir = -lightDir;

    float3 planetCenter = float3(0, -_PlanetRadius, 0);
    float2 intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius + _AtmosphereHeight);
    float3 rayEnd = rayStart + rayDir * intersection.y;

	// compute density along the ray
    float stepCount = 50; // 250;
    float3 step = (rayEnd - rayStart) / stepCount;
    float stepSize = length(step);
    float2 density = 0;

    for (float s = 0.5; s < stepCount; s += 1.0)
    {
        float3 position = rayStart + step * s;
        float height = abs(length(position - planetCenter) - _PlanetRadius);
        float2 localDensity = exp(-(height.xx / _DensityScaleHeight));

        density += localDensity * stepSize;
    }

    opticalDepthCP = density;

    return true;
}

bool GetAtmosphereDensityRealtime(float3 position, float3 planetCenter, float3 lightDir, out float2 dpa, out float2 dpc)
{
    float height = length(position - planetCenter) - _PlanetRadius;
    dpa = exp(-height.xx / _DensityScaleHeight.xy);

    bool bOverGround = lightSampleing(position, lightDir, dpc);
    return bOverGround;
}

void ComputeLocalInscattering(float2 localDensity, float2 densityPA, float2 densityCP, out float3 localInscatterR, out float3 localInscatterM)
{
    float2 densityCPA = densityCP + densityPA;

    float3 Tr = densityCPA.x * _ExtinctionR;
    float3 Tm = densityCPA.y * _ExtinctionM;

    float3 extinction = exp(-(Tr + Tm));

    localInscatterR = localDensity.x * extinction;
    localInscatterM = localDensity.y * extinction;
}
void ApplyPhaseFunction(inout float3 scatterR, inout float3 scatterM, float cosAngle)
{
	// r
    float phase = (3.0 / (16.0 * PI)) * (1 + (cosAngle * cosAngle));
    scatterR *= phase;

	// m
    float g = _MieG;
    float g2 = g * g;
    phase = (1.0 / (4.0 * PI)) * ((3.0 * (1.0 - g2)) / (2.0 * (2.0 + g2))) * ((1 + cosAngle * cosAngle) / (pow((1 + g2 - 2 * g * cosAngle), 3.0 / 2.0)));
    scatterM *= phase;
}

float4 IntegrateInscatteringRealtime(float3 rayStart, float3 rayDir, float rayLength, float3 planetCenter, float distanceScale, float3 lightDir, float sampleCount, out float4 extinction)
{
    float3 step = rayDir * (rayLength / sampleCount);
    float stepSize = length(step) * distanceScale;

    float2 densityPA = 0;
    float3 scatterR = 0;
    float3 scatterM = 0;

    float2 localDensity;
    float2 densityCP;

    float2 prevLocalDensity;
    float3 prevLocalInscatterR, prevLocalInscatterM;
    GetAtmosphereDensityRealtime(rayStart, planetCenter, lightDir, prevLocalDensity, densityCP);

    ComputeLocalInscattering(prevLocalDensity, densityCP, densityPA, prevLocalInscatterR, prevLocalInscatterM);

	// P - current integration point
	// A - camera position
	// C - top of the atmosphere
	[loop]
    for (float s = 1.0; s < sampleCount; s += 1)
    {
        float3 p = rayStart + step * s;

        GetAtmosphereDensityRealtime(p, planetCenter, lightDir, localDensity, densityCP);
        densityPA += (localDensity + prevLocalDensity) * (stepSize / 2.0);
        float3 localInscatterR, localInscatterM;
        ComputeLocalInscattering(localDensity, densityCP, densityPA, localInscatterR, localInscatterM);

        scatterR += (localInscatterR + prevLocalInscatterR) * (stepSize / 2.0);
        scatterM += (localInscatterM + prevLocalInscatterM) * (stepSize / 2.0);

        prevLocalInscatterR = localInscatterR;
        prevLocalInscatterM = localInscatterM;

        prevLocalDensity = localDensity;
    }

    float3 m = scatterM;
	// phase function
    ApplyPhaseFunction(scatterR, scatterM, dot(rayDir, -lightDir.xyz));
	//scatterR = 0;
    float3 lightInscatter = (scatterR * _ScatteringR + scatterM * _ScatteringM) * _IncomingLight.xyz;
	//lightInscatter += RenderSun(m, dot(rayDir, -lightDir.xyz)) * _SunIntensity;
    float3 lightExtinction = exp(-(densityCP.x * _ExtinctionR + densityCP.y * _ExtinctionM));

    extinction = float4(lightExtinction, 0);
    return float4(lightInscatter, 1);
}

float getDensity(float3 pos)
{
    //高さカリング
    float mid = (bottom + top) / 2.0;
    float h = top - bottom;
    float weight = 1.0 - 2.0 * abs(mid - pos.y) / h;
    weight = pow(weight, 0.5);
  
    //float2 coord = pos.xz * 0.0025;
    //float Noise = g_PerlinNoise.Sample(g_SamplerState, coord).x;
    //Noise += g_PerlinNoise.Sample(g_SamplerState, coord * 3.5).x / 3.5;
    //Noise += g_PerlinNoise.Sample(g_SamplerState, coord * 12.25).x / 12.25;
    //Noise += g_PerlinNoise.Sample(g_SamplerState, coord * 42.87).x / 42.87;
    //Noise *= weight;
    pos = pos * 0.04;
    pos.x += (float) Light.frameCount * 0.005;
    pos.y -= (float) Light.frameCount * 0.001;
    float Noise = snoise(pos);
    Noise += snoise(pos * 3.5) / 3.5;
    Noise += snoise(pos * 12.25) / 12.25;
    Noise += snoise(pos * 42.87) / 42.87;
    Noise /= 1.4472;
    Noise *= weight;
    //float dist = abs(pos.y - mid);
    //Noise *= smoothstep(0, 1, h / 2 - dist) * 0.8;
    if(Noise<0.1)
        Noise = 0.0;
    return Noise;
}
//ray matching
float4 getCloud(float3 worldPos, float3 cameraPos)
{
	float3 direction = normalize(worldPos - cameraPos);
    float3 step = direction * 0.15;
	float4 colorSum = float4(0.0, 0.0, 0.0, 0.0);
	float3 testPoint = cameraPos;

	if (testPoint.y < bottom) {
        testPoint += direction * (abs(bottom - testPoint.y) / abs(direction.y));
    }
	else if (top < testPoint.y) {
        testPoint += direction * (abs(testPoint.y - top) / abs(direction.y));
    }
    if (testPoint.x < -width)
    {
        testPoint += direction * (abs(-width - testPoint.x) / abs(direction.x));
    }
    else if (width < testPoint.x)
    {
        testPoint += direction * (abs(testPoint.x - width) / abs(direction.x));
    }
    if (testPoint.z < -width)
    {
        testPoint += direction * (abs(-width - testPoint.z) / abs(direction.z));
    }
    else if (width < testPoint.z)
    {
        testPoint += direction * (abs(testPoint.z - width) / abs(direction.z));
    }

	float len1 = length(testPoint - cameraPos);
	float len2 = length(worldPos - cameraPos);  
	if (len2 < len1) {
		return float4(0.0f,0.0f,0.0f,0.0f);
	}

	// ray marching
	for (int i = 0; i < 130; i++)
	{
        testPoint += step * (1.0 + i * 0.05);
		if (bottom > testPoint.y || testPoint.y > top || 
            -width > testPoint.x || testPoint.x > width || 
            -width > testPoint.z || testPoint.z > width) 
        {
			break;
		}
        float2 mid_width = float2((top + bottom) / 2, (top - bottom));
        float density = getDensity(testPoint) * 0.3f;
        float3 base = lerp(baseBright, baseDark, density) * density;
        float4 color = float4(base, density);
        colorSum = color * (1.0 - colorSum.a) + colorSum;
        
        //float2 mid_width = float2((top + bottom) / 2, (top - bottom));
        //float density =getDensity(testPoint);
        //float3 L = normalize(Light.Direction.xyz*100.0 - testPoint);
        //float lightDensity = getDensity(testPoint + L);
        //float delta = clamp(density - lightDensity, 0.0, 1.0);
        //density *= 0.5;
        
        //float3 base = lerp(baseBright, baseDark, density) * density;
        //float3 light = lerp(lightDark, lightBright, delta);
        //float4 color = float4(base * light, density);
        //colorSum = color * (1.0 - colorSum.a) + colorSum;
        if (colorSum.a >= 0.95)
            break;
    }
    

	return colorSum;
}

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse *= In.Diffuse;

	//float2 xy = In.WorldPosition.xy * 2 / float2(ScreenHeight, ScreenHeight);
    float4 cloud = getCloud(In.WorldPosition.xyz, Camera.Position.xyz);
	//outDiffuse = outDiffuse* float4(Get3DCloudColor(xy),1.0);
    outDiffuse.rgb = outDiffuse.rgb * (1.0f - cloud.a) + cloud.rgb;
    
    
    
    
   // float3 positionWorldSpace = In.WorldPosition.xyz;

   // float3 rayStart = Camera.Position.xyz;
   // float3 rayDir = positionWorldSpace - Camera.Position.xyz;
   // float rayLength = length(rayDir);
   // rayDir /= rayLength;
    
   // float3 planetCenter = float3(0, -_PlanetRadius, 0);
   // float2 intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius + _AtmosphereHeight);

   // rayLength = min(intersection.y, rayLength);

   // float4 extinction;
   // //_SunIntensity = 0;

   
   //float4 inscattering = IntegrateInscatteringRealtime(rayStart, rayDir, rayLength, planetCenter, 1, Light.Direction.xyz, 16, extinction);
   ////outDiffuse *= inscattering;
    
}


