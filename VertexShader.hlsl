#include "ShaderCommon.hlsli"

PS_IN main(VS_IN input)
{
    PS_IN output;
    output.pos = float4(input.pos, 1.0f);
    return output;
}
