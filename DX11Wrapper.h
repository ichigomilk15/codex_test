#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "Quad.h"

class DX11Wrapper
{
public:
    DX11Wrapper();
    ~DX11Wrapper();

    // DirectX11 デバイスとスワップチェーンを初期化
    bool Initialize(HWND hwnd, int width, int height);
    // 画面を指定色でクリア
    void Clear(float r, float g, float b, float a);
    // バックバッファを表示
    void Present();
    // 四角形を描画
    void Draw(float angle);
    // ImGui の描画処理
    void RenderImGui();
    // ウィンドウリサイズ時の処理
    bool Resize(int width, int height);

    constexpr bool IsInitialized() const noexcept { return isInitialized_; }
private:
    // 四角形クラス
    Quad quad_;
private:
    // DirectX11 関連オブジェクト
    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;

    bool isInitialized_ = false;
};
