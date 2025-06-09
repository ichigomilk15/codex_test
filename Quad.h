#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <DirectXMath.h>

// 単純な頂点データを表す構造体
struct SimpleVertex
{
    float x, y, z; // 位置座標
};

class Quad
{
public:
    Quad() = default;
    bool Initialize(ID3D11Device* device);
    void Draw(ID3D11DeviceContext* context, float angle);

private:
    bool CreateResources(ID3D11Device* device);

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;
};

