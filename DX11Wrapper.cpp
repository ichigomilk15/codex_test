#include "pch.h"
#include "DX11Wrapper.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <dxgi.h>
#include <d3dcompiler.h>

DX11Wrapper::DX11Wrapper() {}

DX11Wrapper::~DX11Wrapper()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

bool DX11Wrapper::Initialize(HWND hwnd, int width, int height)
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &scd,
        swapChain_.GetAddressOf(),
        device_.GetAddressOf(),
        nullptr,
        context_.GetAddressOf());

    if (FAILED(hr))
        return false;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D),
                               reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        return false;

    hr = device_->CreateRenderTargetView(backBuffer.Get(), nullptr,
                                         renderTargetView_.GetAddressOf());
    if (FAILED(hr))
        return false;

    context_->OMSetRenderTargets(1, renderTargetView_.GetAddressOf(), nullptr);

    D3D11_VIEWPORT vp;
    vp.Width = static_cast<float>(width);
    vp.Height = static_cast<float>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    context_->RSSetViewports(1, &vp);

    if (!CreateTriangle())
        return false;

    // ImGui initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device_.Get(), context_.Get());

    isInitialized_ = true;
    return true;
}

bool DX11Wrapper::CreateTriangle()
{
    SimpleVertex vertices[] = {
        { 0.0f, 0.5f, 0.0f },
        { 0.5f, -0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f },
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    HRESULT hr = device_->CreateBuffer(&bd, &initData, vertexBuffer_.GetAddressOf());
    if (FAILED(hr))
        return false;

    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    hr = D3DReadFileToBlob(L"VertexShader.cso", vsBlob.GetAddressOf());
    if (FAILED(hr))
        return false;

    hr = device_->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, vertexShader_.GetAddressOf());
    if (FAILED(hr))
        return false;

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    hr = device_->CreateInputLayout(layout, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), inputLayout_.GetAddressOf());
    if (FAILED(hr))
        return false;

    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    hr = D3DReadFileToBlob(L"PixelShader.cso", psBlob.GetAddressOf());
    if (FAILED(hr))
        return false;

    hr = device_->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, pixelShader_.GetAddressOf());
    if (FAILED(hr))
        return false;

    return true;
}

void DX11Wrapper::Clear(float r, float g, float b, float a)
{
    const float color[] = { r, g, b, a };
    context_->ClearRenderTargetView(renderTargetView_.Get(), color);
}

void DX11Wrapper::Draw()
{
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    context_->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
    context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context_->IASetInputLayout(inputLayout_.Get());

    context_->VSSetShader(vertexShader_.Get(), nullptr, 0);
    context_->PSSetShader(pixelShader_.Get(), nullptr, 0);

    context_->Draw(3, 0);
}

void DX11Wrapper::RenderImGui()
{
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DX11Wrapper::Present()
{
    swapChain_->Present(1, 0);
}

bool DX11Wrapper::Resize(int width, int height)
{
    if (!swapChain_)
        return false;

    context_->OMSetRenderTargets(0, nullptr, nullptr);
    renderTargetView_.Reset();

    HRESULT hr = swapChain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr))
        return false;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D),
                               reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        return false;

    hr = device_->CreateRenderTargetView(backBuffer.Get(), nullptr,
                                         renderTargetView_.GetAddressOf());
    if (FAILED(hr))
        return false;

    context_->OMSetRenderTargets(1, renderTargetView_.GetAddressOf(), nullptr);

    D3D11_VIEWPORT vp{};
    vp.Width = static_cast<float>(width);
    vp.Height = static_cast<float>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    context_->RSSetViewports(1, &vp);

    return true;
}
