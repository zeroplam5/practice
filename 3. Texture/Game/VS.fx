
cbuffer cbView : register(b0)
{
    matrix View;
};
cbuffer cbProjection : register(b1)
{
    matrix Projection;
};
cbuffer cbWorld : register(b2)
{
    matrix World;
};


struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Position = mul(Projection, mul(View, mul(World, float4(input.Position, 1.0f))));

    output.TexCoord = input.TexCoord;
    return output;
}


Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);
float4 PS(PS_INPUT input) : SV_Target
{
    return txDiffuse.Sample(samLinear, input.TexCoord);
}
