
#include "common.hlsl"
#include "PerlinNoise.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);


 

float getDensity(float3 pos)
{
    //çÇÇ≥ÉJÉäÉìÉO
    float mid = (bottom + top) / 2.0;
    float h = top - bottom;
    float weight = 1.0 - 2.0 * abs(mid - pos.y) / h;
    weight = pow(weight, 0.5);
  
    pos = pos * 0.04;
    pos.x += (float) Light.frameCount * cloud.speed * cloud.windDir.x;
    pos.y -= (float) Light.frameCount * cloud.speed * cloud.windDir.y;
    float Noise = snoise(pos);
    Noise += snoise(pos * 3.5) / 3.5;
    Noise += snoise(pos * 12.25) / 12.25;
    Noise += snoise(pos * 42.87) / 42.87;
    Noise /= 1.4472;
    Noise *= weight;
    if (Noise < 0.1)
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
    
    if (testPoint.y < bottom)
    {
        testPoint += direction * (abs(bottom - testPoint.y) / abs(direction.y));
    }
    else if (top < testPoint.y)
    {
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
    if (len2 < len1)
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

	// ray marching
    for (int i = 0; i < cloud.matchingcount; i++)
    {
        testPoint += step * (0.1  + i * cloud.matchingstep);
        if (bottom > testPoint.y || testPoint.y > top || -width > testPoint.x || testPoint.x > width || -width > testPoint.z || testPoint.z > width)
        {
            break;
        }
        
        float2 mid_width = float2((top + bottom) / 2, (top - bottom));
        float density = getDensity(testPoint) * 0.3f;
        float3 base = lerp(baseBright, baseDark, density) * density;
        float4 color = float4(base, density);
        colorSum = color * (1.0 - colorSum.a) + colorSum;
        
        //float2 mid_width = float2((top + bottom) / 2, (top - bottom));
        //float density = getDensity(testPoint);
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

 
float Scale(float fcos)
{
    float x = 1.0 - fcos;
    return Atmos.fScaleDepth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.8 + x * 5.25))));
}
 

float3 IntersectionPos(float3 dir, float3 a, float radius)
{
    float b = dot(a, dir);
    float c = dot(a, a) - radius * radius;
    float d = max(b * b - c, 0.0);
 
    return a + dir * (-b + sqrt(d));
}


void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float3 three_primary_colors = float3(0.68, 0.55, 0.44);
    float3 InvWaveLength = 1.0 / pow(three_primary_colors, 4.0);
    
    float fOuterRadius = Atmos._OuterRadius;
    float fInnerRadius = Atmos._InnerRadius;
    
    float fESun = 20.0;
    float fKrESun = Atmos._Kr * fESun;
    float fKmESun = Atmos._Km * fESun;
    
    float fKr4PI = Atmos._Kr * 4.0 * PI;
    float fKm4PI = Atmos._Km * 4.0 * PI;
    
    float fScale = 1.0 / (Atmos._OuterRadius - Atmos._InnerRadius);
    float fScaleOverScaleDepth = fScale / Atmos.fScaleDepth;
    
    float g = -0.999f;
    float g2 = g * g;
    
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse *= In.Diffuse;
    
   
    float3 worldPos = In.WorldPos.xyz;
    float3 CameraPos = float3(Camera.Position.x, Camera.Position.y + fInnerRadius, Camera.Position.z);
    worldPos = IntersectionPos(normalize(worldPos), CameraPos, fOuterRadius);
    float3 LightDir = normalize(-Light.Direction.xyz); //-Light.Direction.xyz;
 
    
    float3 Ray = worldPos - CameraPos;
    float fFar = length(Ray);
    Ray /= fFar;
 
    float3 Start = CameraPos;
    float fCameraHeight = length(CameraPos);
    float fStartAngle = dot(Ray, Start) / fCameraHeight;
    float fStartDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
    float fStartOffset = fStartDepth * Scale(fStartAngle);
 
    float fSampleLength = fFar / Atmos.fSamples;
    float fScaledLength = fSampleLength * fScale;
    float3 SampleRay = Ray * fSampleLength;
    float3 SamplePoint = Start + SampleRay * 0.5;
 
    float3 FrontColor = 0.0;
    for (int n = 0; n < int(Atmos.fSamples); n++)
    {
        float fHeight = length(SamplePoint);
        float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
        float fLightAngle = dot(LightDir, SamplePoint) / fHeight;
        float fCameraAngle = dot(Ray, SamplePoint) / fHeight;
        float fScatter = (fStartOffset + fDepth * (Scale(fLightAngle) - Scale(fCameraAngle)));
        float3 Attenuate = exp(-fScatter * (InvWaveLength * fKr4PI + fKm4PI));
        FrontColor += Attenuate * (fDepth * fScaledLength);
        SamplePoint += SampleRay;
    }
 
    float3 c0 = FrontColor * (InvWaveLength * fKrESun);
    float3 c1 = FrontColor * fKmESun;
    float3 Direction = CameraPos - worldPos;
 
    float fcos = dot(LightDir, Direction) / length(Direction);
    float fcos2 = fcos * fcos;
 
    float rayleighPhase = 0.75 * (1.0 + fcos2);
    float miePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fcos2) / pow(1.0 + g2 - 2.0 * g * fcos, 1.5);
 
    float4 col = 1.0;
    col.rgb = rayleighPhase * c0 + miePhase * c1;
    //col = max(float4(0.0,0.0,0.0,1.0),col);
    outDiffuse = col;
    //if (fKrESun < 0.0)
    //    outDiffuse = float4(0.0, 0.0, 0.0, 1.0);
    //volume cloud
    float4 cloud = getCloud(In.WorldPosition.xyz, Camera.Position.xyz);
    outDiffuse.rgb = outDiffuse.rgb * (1.0f - cloud.a) + cloud.rgb;
}