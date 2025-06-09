#include "pch.h"
#include "DX11Wrapper.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <dxgi.h>
#include <d3dcompiler.h>

// コンストラクタ
DX11Wrapper::DX11Wrapper() {}

// デストラクタでは ImGui の後片付けを行う
DX11Wrapper::~DX11Wrapper()
{
    ImGui_ImplDX11_Shutdown();    // ImGui DX11 バックエンド終了
    ImGui_ImplWin32_Shutdown();   // Win32 バックエンド終了
    ImGui::DestroyContext();      // コンテキスト破棄
}

// DirectX11 の初期化を行う
bool DX11Wrapper::Initialize(HWND hwnd, int width, int height)
{
    // スワップチェーン設定
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    // デバイスとスワップチェーンの作成
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

    // バックバッファ取得
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D),
                               reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        return false;

    // 新しいレンダーターゲットビュー作成
    hr = device_->CreateRenderTargetView(backBuffer.Get(), nullptr,
                                         renderTargetView_.GetAddressOf());
    if (FAILED(hr))
        return false;

    // レンダーターゲットを設定
    context_->OMSetRenderTargets(1, renderTargetView_.GetAddressOf(), nullptr);

    // ビューポート設定
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<float>(width);
    vp.Height = static_cast<float>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    context_->RSSetViewports(1, &vp);

    // 四角形の生成
    if (!quad_.Initialize(device_.Get()))
        return false;

    // ImGui 初期化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device_.Get(), context_.Get());

    isInitialized_ = true;
    return true;
}


// 画面を指定色でクリア
void DX11Wrapper::Clear(float r, float g, float b, float a)
{
    const float color[] = { r, g, b, a };
    context_->ClearRenderTargetView(renderTargetView_.Get(), color);
}

// 頂点バッファを利用して描画
void DX11Wrapper::Draw(float angle)
{
    quad_.Draw(context_.Get(), angle);
}

// ImGui の描画データを送信
void DX11Wrapper::RenderImGui()
{
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

// 表示を更新
void DX11Wrapper::Present()
{
    swapChain_->Present(1, 0);
}

// ウィンドウサイズ変更に対応
bool DX11Wrapper::Resize(int width, int height)
{
    if (!swapChain_)
        return false; // 初期化前

    // 既存リソースを解放
    context_->OMSetRenderTargets(0, nullptr, nullptr);
    renderTargetView_.Reset();

    // バックバッファを再生成
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

    // ビューポートを再設定
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
