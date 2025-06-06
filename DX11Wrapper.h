#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class DX11Wrapper
{
public:
    DX11Wrapper();
    ~DX11Wrapper();

    bool Initialize(HWND hwnd, int width, int height);
    void Clear(float r, float g, float b, float a);
    void Present();

    constexpr bool IsInitialized() const noexcept { return isInitialized_; }
private:
    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;

    bool isInitialized_ = false;
};
