
#include "common.hlsl"
#include "PerlinNoise.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

uniform float2 iResolution = float2(ScreenWidth, ScreenHeight); // viewport resolution (in pixels)
float iTime=0.0; // shader playback time (in seconds)
uniform float4 iMouse; // mouse pixel coords. xy: current (if MLB down), zw: click

// 0: one 3d texture lookup
// 1: two 2d texture lookups with hardware interpolation
// 2: two 2d texture lookups with software interpolation
#define NOISE_METHOD 1

// 0: no LOD
// 1: yes LOD
#define USE_LOD 1

float map(in float3 p, int oct)
{
    float3 q = p - float3(0.0, 0.1, 1.0) * iTime;
    float g = 0.5 + 0.5 * snoise(q * 0.3);
    
    float f;
    f = 0.50000 * snoise(q);
    q = q * 2.02;
#if USE_LOD==1
    if( oct>=2 ) 
#endif
    f += 0.25000 * snoise(q);
    q = q * 2.23;
#if USE_LOD==1
    if( oct>=3 )
#endif
    f += 0.12500 * snoise(q);
    q = q * 2.41;
#if USE_LOD==1
    if( oct>=4 )
#endif
    f += 0.06250 * snoise(q);
    q = q * 2.62;
#if USE_LOD==1
    if( oct>=5 )
#endif
    f += 0.03125 * snoise(q);
    
    f = lerp(f * 0.1 - 0.75, f, g * g) + 0.1;
    return 1.5 * f - 0.5 - p.y;
}

const float3 sundir = normalize(float3(-1.0, 0.0, -1.0));
const int kDiv = 1; // make bigger for higher quality

float4 raymarch(in float3 ro, in float3 rd, in float3 bgcol, in float2 px)
{
    // bounding planes	
    const float yb = -3.0;
    const float yt = 0.6;
    float tb = (yb - ro.y) / rd.y;
    float tt = (yt - ro.y) / rd.y;

    // find tigthest possible raymarching segment
    float tmin, tmax;
    if (ro.y > yt)
    {
        // above top plane
        if (tt < 0.0)
            return float4(0.0,0.0,0.0,0.0); // early exit
        tmin = tt;
        tmax = tb;
    }
    else
    {
        // inside clouds slabs
        tmin = 0.0;
        tmax = 60.0;
        if (tt > 0.0)
            tmax = min(tmax, tt);
        if (tb > 0.0)
            tmax = min(tmax, tb);
    }
    
    // dithered near distance
    float t = tmin + 0.1 * snoise(float3((int) px.x & 1023, (int) px.y & 1023, 0.0));
    
    // raymarch loop
    float4 sum = float4(0.0,0.0,0.0,0.0);
    for (int i = 0; i < 190 * kDiv; i++)
    {
       // step size
        float dt = max(0.05, 0.02 * t / float(kDiv));

       // lod
#if USE_LOD==0
        const int oct = 5;
#else
       int oct = 5 - int( log2(1.0+t*0.5) );
#endif
       
       // sample cloud
        float3 pos = ro + t * rd;
        float den = map(pos, oct);
        if (den > 0.01) // if inside
        {
           // do lighting
            float dif = clamp((den - map(pos + 0.3 * sundir, oct)) / 0.3, 0.0, 1.0);
            float3 lin = float3(0.65, 0.65, 0.75) * 1.1 + 0.8 * float3(1.0, 0.6, 0.3) * dif;
            float4 col = float4(lerp(float3(1.0, 0.95, 0.8), float3(0.25, 0.3, 0.35), den), den);
            col.xyz *= lin;
           // fog
            col.xyz = lerp(col.xyz, bgcol, 1.0 - exp2(-0.075 * t));
           // composite front to back
            col.w = min(col.w * 8.0 * dt, 1.0);
            col.rgb *= col.a;
            sum += col * (1.0 - sum.a);
        }
       // advance ray
        t += dt;
       // until far clip or full opacity
        if (t > tmax || sum.a > 0.99)
            break;
    }
    return clamp(sum, 0.0, 1.0);
}

matrix<float, 3, 3> setCamera(in float3 ro, in float3 ta, float cr)
{
    float3 cw = normalize(ta - ro);
    float3 cp = float3(sin(cr), cos(cr), 0.0);
    float3 cu = normalize(cross(cw, cp));
    float3 cv = normalize(cross(cu, cw));
    matrix<float, 3, 3> cam;
    cam._11_12_13 = cu;
    cam._21_22_23 = cv;
    cam._31_32_33 = cw;
    return cam;
}

float4 render(in float3 ro, in float3 rd, in float2 px)
{
    float sun = clamp(dot(sundir, rd), 0.0, 1.0);

    // background sky
    float3 col = float3(0.76, 0.75, 0.86);
    col -= 0.6 * float3(0.90, 0.75, 0.95) * rd.y;
    col += 0.2 * float3(1.00, 0.60, 0.10) * pow(sun, 8.0);

    // clouds    
    float4 res = raymarch(ro, rd, col, px);
    col = col * (1.0 - res.w) + res.xyz;
    
    // sun glare    
    col += 0.2 * float3(1.0, 0.4, 0.2) * pow(sun, 3.0);

    // tonemap
    col = smoothstep(0.15, 1.1, col);
 
    return float4(col, 1.0);
}

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    
    float2 p = (2.0 * In.TexCoord - iResolution.xy) / iResolution.y;
    float2 m = iMouse.xy / iResolution.xy;
    
    // camera
    float3 ro = 4.0 * normalize(float3(sin(3.0 * m.x), 0.8 * m.y, cos(3.0 * m.x))) - float3(0.0, 0.1, 0.0);
    float3 ta = float3(0.0, -1.0, 0.0);
    float3x3 ca = setCamera(ro, ta, 0.07 * cos(0.25 * iTime));
    // ray
    float3 rd = Camera.Position.xyz * normalize(float3(p.xy, 1.5));
    
    In.iTime+=0.1;
    outDiffuse = render(ro, rd, float2(In.TexCoord - 0.5));
}
