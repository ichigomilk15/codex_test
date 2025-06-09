#include "pch.h"
#include "Polygon.h"
#include <string>

using Microsoft::WRL::ComPtr;

static bool CompileShader(const wchar_t* file, const char* entry, const char* target, ComPtr<ID3DBlob>& blob)
{
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3DCOMPILE_DEBUG;
#endif
    ComPtr<ID3DBlob> error;
    HRESULT hr = D3DCompileFromFile(file, nullptr, nullptr, entry, target, flags, 0, blob.GetAddressOf(), error.GetAddressOf());
    if (FAILED(hr)) {
        if (error) OutputDebugStringA((char*)error->GetBufferPointer());
        return false;
    }
    return true;
}

bool Polygon::Initialize(ID3D11Device* device)
{
    CreateVertices();
    return CreateResources(device);
}

bool Polygon::CreateResources(ID3D11Device* device)
{
    if (vertices_.empty()) return false;

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices_.size());
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA init = { vertices_.data(), 0, 0 };
    if (FAILED(device->CreateBuffer(&bd, &init, vertexBuffer_.GetAddressOf())))
        return false;

    ComPtr<ID3DBlob> vsBlob;
    if (!CompileShader(L"VertexShader.hlsl", "main", "vs_5_0", vsBlob))
        return false;
    if (FAILED(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, vertexShader_.GetAddressOf())))
        return false;

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    if (FAILED(device->CreateInputLayout(layout, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), inputLayout_.GetAddressOf())))
        return false;

    ComPtr<ID3DBlob> psBlob;
    if (!CompileShader(L"PixelShader.hlsl", "main", "ps_5_0", psBlob))
        return false;
    if (FAILED(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, pixelShader_.GetAddressOf())))
        return false;

    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    if (FAILED(device->CreateBuffer(&cbd, nullptr, vsConstantBuffer_.GetAddressOf())))
        return false;

    cbd.ByteWidth = sizeof(DirectX::XMFLOAT4);
    if (FAILED(device->CreateBuffer(&cbd, nullptr, psConstantBuffer_.GetAddressOf())))
        return false;

    return true;
}

void Polygon::Draw(ID3D11DeviceContext* context)
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
    context->IASetPrimitiveTopology(topology_);
    context->IASetInputLayout(inputLayout_.Get());

    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationZ(rotation.z);
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    DirectX::XMMATRIX world = S * R * T;
    DirectX::XMFLOAT4X4 mat;
    DirectX::XMStoreFloat4x4(&mat, DirectX::XMMatrixTranspose(world));
    context->UpdateSubresource(vsConstantBuffer_.Get(), 0, nullptr, &mat, 0, 0);
    context->VSSetConstantBuffers(0, 1, vsConstantBuffer_.GetAddressOf());

    context->UpdateSubresource(psConstantBuffer_.Get(), 0, nullptr, color, 0, 0);
    context->PSSetConstantBuffers(1, 1, psConstantBuffer_.GetAddressOf());

    context->VSSetShader(vertexShader_.Get(), nullptr, 0);
    context->PSSetShader(pixelShader_.Get(), nullptr, 0);
    context->Draw(static_cast<UINT>(vertices_.size()), 0);
}

void Polygon::DrawGUI()
{
    ImGui::ColorEdit4("Color", color);
    ImGui::SliderAngle("Rotation Z", &rotation.z, -180.0f, 180.0f);
    ImGui::DragFloat3("Scale", &scale.x, 0.1f);
    ImGui::DragFloat3("Position", &position.x, 0.1f);
}

