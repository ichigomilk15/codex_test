#include "ShaderCommon.hlsli"

PS_IN VSMain(VS_IN input)
{
    PS_IN output;
    output.pos = float4(input.pos, 1.0f);
    return output;
}
