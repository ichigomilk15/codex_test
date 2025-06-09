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
    hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        return false;

    hr = device_->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView_.GetAddressOf());
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device_.Get(), context_.Get());

    AddPolygon(std::make_unique<Quad>());

    isInitialized_ = true;
    return true;
}

void DX11Wrapper::AddPolygon(std::unique_ptr<Polygon> poly)
{
    if (poly && poly->Initialize(device_.Get()))
        polygons_.push_back(std::move(poly));
}

void DX11Wrapper::Clear(float r, float g, float b, float a)
{
    const float color[] = { r, g, b, a };
    context_->ClearRenderTargetView(renderTargetView_.Get(), color);
}

void DX11Wrapper::Draw()
{
    for (auto& p : polygons_)
        p->Draw(context_.Get());
}

void DX11Wrapper::DrawGUI()
{
    ImGui::Begin("Polygons");
    if (ImGui::Button("Add Triangle")) AddPolygon(std::make_unique<Triangle>());
    ImGui::SameLine();
    if (ImGui::Button("Add Quad")) AddPolygon(std::make_unique<Quad>());
    ImGui::SameLine();
    if (ImGui::Button("Add Pentagon")) AddPolygon(std::make_unique<Pentagon>());

    for (size_t i = 0; i < polygons_.size(); )
    {
        ImGui::PushID(static_cast<int>(i));
        if (ImGui::TreeNode(polygons_[i]->GetName()))
        {
            polygons_[i]->DrawGUI();
            if (ImGui::Button("Remove"))
            {
                polygons_.erase(polygons_.begin() + i);
                ImGui::TreePop();
                ImGui::PopID();
                continue;
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
        ++i;
    }
    ImGui::End();
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
    hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        return false;

    hr = device_->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView_.GetAddressOf());
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
