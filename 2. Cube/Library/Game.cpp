#include "Game.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>

using namespace DirectX;
// ������ Ŭ���� �̸� ����
LPCWSTR g_pszWindowClassName = L"WindowClass";
LPCWSTR g_pszWindowName = L"Direct3D 11 Cube";

// ���� ����
HWND g_hWnd = nullptr;
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
ID3D11Texture2D* g_pDepthStencil = nullptr;

ID3D11Buffer* g_pVertexBuffer = nullptr;
ID3D11Buffer* g_pIndexBuffer = nullptr;
ID3D11Buffer* g_pConstantBuffer = nullptr;
ID3D11InputLayout* g_pVertexLayout = nullptr;

ID3D11VertexShader* g_pVertexShader = nullptr;
ID3D11PixelShader* g_pPixelShader = nullptr;

XMMATRIX g_worldMatrix;
XMMATRIX g_viewMatrix;
XMMATRIX g_projectionMatrix;



HRESULT CompileShaderFromFile(PCWSTR fileName, PCSTR entryPoint, PCSTR shaderModel, ID3DBlob** blobOut)
{
    HRESULT hr = S_OK;
    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
    shaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* errorBlob = nullptr;
    hr = D3DCompileFromFile(fileName, nullptr, nullptr, entryPoint, shaderModel, shaderFlags, 0, blobOut, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
    }
    if (errorBlob) errorBlob->Release();
    return hr;
}





HRESULT InitDevice() {
    HRESULT hr = S_OK;

    // ������ ũ�� ����
    RECT rc;
    GetClientRect(g_hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        1,
        D3D11_SDK_VERSION,
        &sd,
        &g_pSwapChain,
        &g_pd3dDevice,
        &featureLevel,
        &g_pImmediateContext
    );
    if (FAILED(hr)) return hr;

    // ����� ����
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr)) return hr;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr)) return hr;

    // DepthStencil ����
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = g_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &g_pDepthStencil);
    if (FAILED(hr)) return hr;

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
    if (FAILED(hr)) return hr;

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    // ����Ʈ
    D3D11_VIEWPORT vp = {};
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports(1, &vp);

    // ���� ����
    SimpleVertex vertices[] = {
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0, 0, 1, 1) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0, 1, 0, 1) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0, 1, 1, 1) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1, 0, 0, 1) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1, 0, 1, 1) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1, 1, 0, 1) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1, 1, 1, 1) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0, 0, 0, 1) },
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &g_pVertexBuffer);
    if (FAILED(hr)) return hr;

    // �ε��� ����
    WORD indices[] = {
        3,1,0, 2,1,3, 0,5,4, 1,5,0,
        3,4,7, 0,4,3, 1,6,5, 2,6,1,
        2,7,6, 3,7,2, 6,4,5, 7,4,6,
    };
    bd.ByteWidth = sizeof(indices);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    initData.pSysMem = indices;
    hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &g_pIndexBuffer);
    if (FAILED(hr)) return hr;

    // ���̴� ������
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    shaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    hr = CompileShaderFromFile(L"VS.fx", "VS", "vs_5_0", &vsBlob);
    if (FAILED(hr)) return hr;

    hr = g_pd3dDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &g_pVertexShader);
    if (FAILED(hr)) return hr;

    hr = CompileShaderFromFile(L"VS.fx", "PS", "ps_5_0", &psBlob);
    if (FAILED(hr)) return hr;

    hr = g_pd3dDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &g_pPixelShader);
    if (FAILED(hr)) return hr;

    // �Է� ���̾ƿ�
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = g_pd3dDevice->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &g_pVertexLayout);
    vsBlob->Release(); psBlob->Release();
    if (FAILED(hr)) return hr;

    // ��� ����
    g_worldMatrix = XMMatrixIdentity();
    XMVECTOR eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    XMVECTOR at = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    g_viewMatrix = XMMatrixLookAtLH(eye, at, up);
    g_projectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, (FLOAT)width / height, 0.01f, 100.0f);

    // Constant Buffer ����
    D3D11_BUFFER_DESC cbd = {};
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = g_pd3dDevice->CreateBuffer(&cbd, nullptr, &g_pConstantBuffer);
    return hr;
}

void Render() {
    float clearColor[4] = { 0.2f, 0.2f, 0.4f, 1.0f };
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);
    g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;

    g_pImmediateContext->IASetInputLayout(g_pVertexLayout);
    g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
    g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
    g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);

    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if (timeStart == 0) timeStart = timeCur;
    float t = (timeCur - timeStart) / 1000.0f;
    // --- [1] ù ��° ť��: ȸ��
    XMMATRIX rotation1 = XMMatrixRotationY(t);
    XMMATRIX translation1 = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

    XMMATRIX worldMatrix1 = rotation1 * translation1;
    ConstantBuffer cb1;
    cb1.World = XMMatrixTranspose(worldMatrix1);
    cb1.View = XMMatrixTranspose(g_viewMatrix);
    cb1.Projection = XMMatrixTranspose(g_projectionMatrix);
    g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb1, 0, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pImmediateContext->DrawIndexed(36, 0, 0);

    // ���� ť�� (2��): �߽� ť�긦 �������� ����
    XMMATRIX rotationAroundCenter = XMMatrixRotationY(t);                     // ���� ȸ��
    XMMATRIX orbitRadius = XMMatrixTranslation(3.0f, 0.0f, 0.0f);             // �߽ɿ��� ������ 3
    XMMATRIX selfRotation = XMMatrixRotationY(t * 5.0f);                      // ����

    XMMATRIX worldMatrix2 = rotationAroundCenter * orbitRadius * selfRotation;
    // ����: ���� �� ������ �̵� �� ����

    ConstantBuffer cb2;
    cb2.World = XMMatrixTranspose(worldMatrix2);
    cb2.View = XMMatrixTranspose(g_viewMatrix);
    cb2.Projection = XMMatrixTranspose(g_projectionMatrix);
    g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb2, 0, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pImmediateContext->DrawIndexed(36, 0, 0);

    g_pSwapChain->Present(0, 0);
}




LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_CLOSE:
        if (MessageBox(hWnd, L"�����Ͻðڽ��ϱ�?", L"Ȯ��", MB_OKCANCEL) == IDOK)
            DestroyWindow(hWnd);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = g_pszWindowClassName;

    if (!RegisterClassEx(&wc))
        return E_FAIL;

    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    g_hWnd = CreateWindow(
        g_pszWindowClassName,
        g_pszWindowName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInstance, nullptr);

    if (!g_hWnd)
        return E_FAIL;

    ShowWindow(g_hWnd, nCmdShow);
    return S_OK;
}

void CleanupDevice() {
    if (g_pImmediateContext) g_pImmediateContext->ClearState();
    if (g_pConstantBuffer) g_pConstantBuffer->Release();
    if (g_pVertexBuffer) g_pVertexBuffer->Release();
    if (g_pIndexBuffer) g_pIndexBuffer->Release();
    if (g_pVertexLayout) g_pVertexLayout->Release();
    if (g_pVertexShader) g_pVertexShader->Release();
    if (g_pPixelShader) g_pPixelShader->Release();
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pDepthStencilView) g_pDepthStencilView->Release();
    if (g_pDepthStencil) g_pDepthStencil->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pImmediateContext) g_pImmediateContext->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
}
