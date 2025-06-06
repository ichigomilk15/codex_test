#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

// 単純な頂点データを表す構造体
struct SimpleVertex
{
    float x, y, z;    // 位置座標
};

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
    // 三角形を描画
    void Draw();
    // ImGui の描画処理
    void RenderImGui();
    // ウィンドウリサイズ時の処理
    bool Resize(int width, int height);

    constexpr bool IsInitialized() const noexcept { return isInitialized_; }
private:
    // 頂点バッファとシェーダを作成
    bool CreateTriangle();
private:
    // DirectX11 関連オブジェクト
    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;

    // 描画用リソース
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;

    bool isInitialized_ = false;
};
