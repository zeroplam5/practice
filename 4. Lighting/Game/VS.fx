// Constant Buffers
cbuffer cbView : register(b0)
{
    matrix View;
    float4 CameraPosition; // 카메라 위치 추가
};

cbuffer cbProjection : register(b1)
{
    matrix Projection;
};

cbuffer cbWorld : register(b2)
{
    matrix World;
};

cbuffer cbLights : register(b3)
{
    float4 LightPositions[2];
    float4 LightColors[2];
};

cbuffer cbChangeEveryFrame : register(b4)
{
    float4 OutColor;
};

// Vertex & Pixel Input Structures
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;
};

// Vertex Shader
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    float4 worldPos = mul(float4(input.Position, 1.0f), World);
    output.Position = mul(Projection, mul(View, worldPos));
    output.WorldPosition = worldPos.xyz;

    output.Normal = normalize(mul(float4(input.Normal, 0.0f), World).xyz);
    output.TexCoord = input.TexCoord;

    return output;
}

// Texture
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

// Pixel Shader (Phong Lighting: Ambient + Diffuse + Specular)
float4 PS(PS_INPUT input) : SV_Target
{
    float4 color = txDiffuse.Sample(samLinear, input.TexCoord);

    float3 ambient = float3(0.1f, 0.1f, 0.1f);
    float3 diffuse = 0;
    float3 specular = 0;

    float3 viewDir = normalize(CameraPosition.xyz - input.WorldPosition);

    for (uint i = 0; i < 2; i++)
    {
        float3 lightDir = normalize(LightPositions[i].xyz - input.WorldPosition);
        float NdotL = max(dot(input.Normal, lightDir), 0.0f);

        // Diffuse
        diffuse += NdotL * LightColors[i].rgb;

        // Specular
        float3 reflectDir = reflect(-lightDir, input.Normal);
        float spec = pow(max(dot(reflectDir, viewDir), 0.0f), 16.0f); // 16: shininess
        specular += spec * LightColors[i].rgb;
    }

    float3 finalLighting = saturate(ambient + diffuse + specular);
    return float4(finalLighting * color.rgb, color.a);
}

float4 PSSolid(PS_INPUT input) : SV_Target
{
    return OutColor;
}
