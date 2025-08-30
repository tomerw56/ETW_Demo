// Minimal D3D11 GPU Activity Demo
// Creates a window, initializes D3D11, renders a rotating triangle to generate GPU work.
// Build (x64 Native Tools Command Prompt):
//   cl /EHsc /O2 /DNOMINMAX gpu_demo.cpp /link d3d11.lib dxgi.lib d3dcompiler.lib user32.lib gdi32.lib
// Run the exe, let it draw for ~15–30 seconds while you record with WPR.


/*! \file
* \brief See Gpu activation -we can analyse it.
*
*  - Execution:
*    -# wpr -start cpu -start gpu -filemode
*    -# gpu_demo.exe
*    -# wpr -stop gpu_demo.etl
*/

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <chrono>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;

static const char* VS_SRC = R"( 
cbuffer CB : register(b0) { float4x4 mvp; };
struct VSIn { float3 pos : POSITION; float3 col : COLOR; };
struct VSOut { float4 pos : SV_POSITION; float3 col : COLOR; };
VSOut main(VSIn i){ VSOut o; o.pos = mul(float4(i.pos,1), mvp); o.col = i.col; return o; }
)";

static const char* PS_SRC = R"(
struct PSIn { float4 pos : SV_POSITION; float3 col : COLOR; };
float4 main(PSIn i) : SV_TARGET { return float4(i.col,1); }
)";

struct Vertex { XMFLOAT3 pos; XMFLOAT3 col; };

struct CB { XMFLOAT4X4 mvp; };

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProc(h, m, w, l);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    // Create window
    WNDCLASS wc{};
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = LPTSTR((LPCTSTR)"D3D11Demo");
    RegisterClass(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, LPTSTR((LPCTSTR)"D3D11 GPU Activity Demo"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, nullptr, nullptr, hInst, nullptr);

    // Create device + swap chain
    DXGI_SWAP_CHAIN_DESC sd{}; sd.BufferCount = 2; sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; sd.OutputWindow = hwnd; sd.SampleDesc.Count = 1; sd.Windowed = TRUE; sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    UINT flags = 0; // D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL fl;
    ComPtr<ID3D11Device> dev; ComPtr<ID3D11DeviceContext> ctx; ComPtr<IDXGISwapChain> sc;
    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0,
        D3D11_SDK_VERSION, &sd, &sc, &dev, &fl, &ctx))) return 0;

    // RT view
    ComPtr<ID3D11Texture2D> bb; sc->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)bb.GetAddressOf());

    ComPtr<ID3D11RenderTargetView> rtv; dev->CreateRenderTargetView(bb.Get(), nullptr, &rtv);
    // Right after creating the render target view (rtv)
    D3D11_VIEWPORT vp{};
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width = 1280.0f;   // match window width
    vp.Height = 720.0f;    // match window height
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    ctx->RSSetViewports(1, &vp);

    // Compile shaders
    ComPtr<ID3DBlob> vsb, psb, err;
    D3DCompile(VS_SRC, strlen(VS_SRC), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsb, &err);
    D3DCompile(PS_SRC, strlen(PS_SRC), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &psb, &err);

    ComPtr<ID3D11VertexShader> vs; dev->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, &vs);
    ComPtr<ID3D11PixelShader> ps; dev->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, &ps);

    // Input layout
    D3D11_INPUT_ELEMENT_DESC il[] = {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,offsetof(Vertex,pos),D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,offsetof(Vertex,col),D3D11_INPUT_PER_VERTEX_DATA,0}
    };
    ComPtr<ID3D11InputLayout> layout; dev->CreateInputLayout(il, 2, vsb->GetBufferPointer(), vsb->GetBufferSize(), &layout);

    // Triangle
    Vertex verts[] = {
        {{ 0.0f,  0.5f, 0.0f}, {1,0,0}},
        {{ 0.5f, -0.5f, 0.0f}, {0,1,0}},
        {{-0.5f, -0.5f, 0.0f}, {0,0,1}}
    };
    D3D11_BUFFER_DESC vbd{}; vbd.ByteWidth = sizeof(verts); vbd.Usage = D3D11_USAGE_DEFAULT; vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vinit{}; vinit.pSysMem = verts;
    ComPtr<ID3D11Buffer> vbo; dev->CreateBuffer(&vbd, &vinit, &vbo);

    // Constant buffer
    D3D11_BUFFER_DESC cbd{}; cbd.ByteWidth = sizeof(CB); cbd.Usage = D3D11_USAGE_DEFAULT; cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    ComPtr<ID3D11Buffer> cbuf; dev->CreateBuffer(&cbd, nullptr, &cbuf);

    // Pipeline setup
    ctx->IASetInputLayout(layout.Get());
    UINT stride = sizeof(Vertex), offset = 0; ctx->IASetVertexBuffers(0, 1, vbo.GetAddressOf(), &stride, &offset);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->VSSetShader(vs.Get(), nullptr, 0); ctx->PSSetShader(ps.Get(), nullptr, 0);
    ctx->VSSetConstantBuffers(0, 1, cbuf.GetAddressOf());

    // Time loop
    auto t0 = std::chrono::high_resolution_clock::now();
    bool running = true; MSG msg{};
    while (running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) running = false; TranslateMessage(&msg); DispatchMessage(&msg);
        }
        // Rotate MVP
        auto t = std::chrono::high_resolution_clock::now();
        float sec = std::chrono::duration<float>(t - t0).count();
        XMMATRIX m = XMMatrixRotationZ(sec) * XMMatrixScaling(0.8f, 0.8f, 1.0f);
        XMMATRIX v = XMMatrixIdentity();
        XMMATRIX p = XMMatrixOrthographicLH(2, 2, 0, 1);
        XMMATRIX mvp = XMMatrixTranspose(m * v * p);
        CB cb; XMStoreFloat4x4(&cb.mvp, mvp);
        ctx->UpdateSubresource(cbuf.Get(), 0, nullptr, &cb, 0, 0);

        // Draw
        FLOAT clear[4] = { 0.06f,0.06f,0.10f,1.0f };
        ctx->OMSetRenderTargets(1, rtv.GetAddressOf(), nullptr);
        ctx->ClearRenderTargetView(rtv.Get(), clear);
        ctx->Draw(3, 0);
        sc->Present(1, 0);
    }

    return 0;
}
