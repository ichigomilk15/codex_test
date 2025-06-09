#include "pch.h"
#include "Quad.h"

bool Quad::Initialize(ID3D11Device* device)
{
    return CreateResources(device);
}

bool Quad::CreateResources(ID3D11Device* device)
{
    // 頂点データ (三角形ストリップ用に 4 点)
    SimpleVertex vertices[] = {
        { -0.5f,  0.5f, 0.0f },
        {  0.5f,  0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f },
        {  0.5f, -0.5f, 0.0f },
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    HRESULT hr = device->CreateBuffer(&bd, &initData, vertexBuffer_.GetAddressOf());
    if (FAILED(hr))
        return false;

    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    hr = D3DReadFileToBlob(L"VertexShader.cso", vsBlob.GetAddressOf());
    if (FAILED(hr))
        return false;
    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, vertexShader_.GetAddressOf());
    if (FAILED(hr))
        return false;

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    hr = device->CreateInputLayout(layout, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), inputLayout_.GetAddressOf());
    if (FAILED(hr))
        return false;

    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    hr = D3DReadFileToBlob(L"PixelShader.cso", psBlob.GetAddressOf());
    if (FAILED(hr))
        return false;
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, pixelShader_.GetAddressOf());
    if (FAILED(hr))
        return false;

    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&cbd, nullptr, constantBuffer_.GetAddressOf());
    if (FAILED(hr))
        return false;

    return true;
}

void Quad::Draw(ID3D11DeviceContext* context, float angle)
{
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->IASetInputLayout(inputLayout_.Get());

    DirectX::XMMATRIX world = DirectX::XMMatrixRotationZ(angle);
    DirectX::XMFLOAT4X4 mat;
    DirectX::XMStoreFloat4x4(&mat, world);
    context->UpdateSubresource(constantBuffer_.Get(), 0, nullptr, &mat, 0, 0);
    context->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());

    context->VSSetShader(vertexShader_.Get(), nullptr, 0);
    context->PSSetShader(pixelShader_.Get(), nullptr, 0);

    context->Draw(4, 0);
}

