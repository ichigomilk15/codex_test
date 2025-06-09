#include "ShaderCommon.hlsli"

cbuffer Transform : register(b0)
{
    float4x4 mtx;
};

PS_IN main(VS_IN input)
{
    PS_IN output;
    output.pos = mul(float4(input.pos, 1.0f), mtx);
    return output;
}
