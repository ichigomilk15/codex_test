#include "pch.h"
#include "Quad.h"

void Quad::CreateVertices()
{
    vertices_ = {
        { -0.5f,  0.5f, 0.0f },
        {  0.5f,  0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f },
        {  0.5f,  0.5f, 0.0f },
        {  0.5f, -0.5f, 0.0f },
    };
    topology_ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}
