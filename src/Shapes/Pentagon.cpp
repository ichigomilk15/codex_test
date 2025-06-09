#include "Pentagon.h"
#include <cmath>

namespace Shapes {

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
    DirectX::XMFLOAT3 center{0.0f, 0.0f, 0.0f};
    for (int i = 0; i < 5; ++i) {
        vertices_.push_back({ center.x, center.y, center.z });
        vertices_.push_back({ pts[i].x, pts[i].y, pts[i].z });
        vertices_.push_back({ pts[(i + 1) % 5].x, pts[(i + 1) % 5].y, pts[(i + 1) % 5].z });
    }
    topology_ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

} // namespace Shapes
