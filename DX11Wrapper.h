#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

struct SimpleVertex
{
    float x, y, z;
};

class DX11Wrapper
{
public:
    DX11Wrapper();
    ~DX11Wrapper();

    bool Initialize(HWND hwnd, int width, int height);
    void Clear(float r, float g, float b, float a);
    void Present();
    void Draw();
    void RenderImGui();
    bool Resize(int width, int height);

    constexpr bool IsInitialized() const noexcept { return isInitialized_; }
private:
    bool CreateTriangle();
private:
    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;

    bool isInitialized_ = false;
};
