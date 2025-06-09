#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "Triangle.h"
#include "Quad.h"
#include "Pentagon.h"

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
    void DrawGUI();
    bool Resize(int width, int height);

    constexpr bool IsInitialized() const noexcept { return isInitialized_; }

    void AddPolygon(std::unique_ptr<Shapes::Polygon> poly);
private:
    std::vector<std::unique_ptr<Shapes::Polygon>> polygons_;

    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;

    bool isInitialized_ = false;
};
