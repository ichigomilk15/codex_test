#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include "imgui.h"

// 汎用多角形基底クラス
class Polygon {
public:
    virtual ~Polygon() = default;
    bool Initialize(ID3D11Device* device);
    virtual const char* GetName() const = 0;
    virtual void Draw(ID3D11DeviceContext* context);
    virtual void DrawGUI();

    DirectX::XMFLOAT3 position{0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 rotation{0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 scale{1.0f, 1.0f, 1.0f};
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};

protected:
    struct Vertex { float x, y, z; };
    std::vector<Vertex> vertices_;
    D3D11_PRIMITIVE_TOPOLOGY topology_ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> psConstantBuffer_;

    virtual void CreateVertices() = 0;
    bool CreateResources(ID3D11Device* device);
};
