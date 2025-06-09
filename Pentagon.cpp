#include "pch.h"
#include "Pentagon.h"
#include <cmath>

void Pentagon::CreateVertices()
{
    const float radius = 0.5f;
    const float step = DirectX::XM_2PI / 5.0f;
    DirectX::XMFLOAT3 pts[5];
    for (int i = 0; i < 5; ++i) {
        float a = step * i - DirectX::XM_PIDIV2;
        pts[i] = { radius * cosf(a), radius * sinf(a), 0.0f };
    }
    vertices_.clear();
    for (int i = 1; i < 4; ++i) {
        vertices_.push_back({ pts[0].x, pts[0].y, pts[0].z });
        vertices_.push_back({ pts[i].x, pts[i].y, pts[i].z });
        vertices_.push_back({ pts[i+1].x, pts[i+1].y, pts[i+1].z });
    }
    topology_ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}
