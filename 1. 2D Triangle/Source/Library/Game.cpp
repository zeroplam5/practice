#include "Game.h"
#include <DirectXMath.h> 
#include <d3d11.h>
#include <d3dcompiler.h>
using namespace DirectX;


LPCWSTR g_pszWindowClassName = L"WindowClass";
HWND g_hWnd = nullptr;
HINSTANCE g_hInstance = nullptr;
LPCWSTR g_pszWindowName = L"GGPO2 : Direct3D 11 Basics";

ID3D11Buffer* g_pVertexBuffer = nullptr;
ID3D11Buffer* g_pIndexBuffer = nullptr;
ID3D11InputLayout* g_pVertexLayout = nullptr;
ID3D11VertexShader* g_pVertexShader = nullptr;
ID3D11PixelShader* g_pPixelShader = nullptr;
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
D3D_FEATURE_LEVEL g_featureLevel;

struct SimpleVertex
{
	DirectX::XMFLOAT3 Pos;
};





HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

    // 1. 스왑체인 설정
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 800;
    sd.BufferDesc.Height = 600;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd; // 반드시 InitWindow 후!
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    // 2. 디바이스 + 스왑체인 한 번에 생성
    hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &sd,
        &g_pSwapChain,
        &g_pd3dDevice,
        &g_featureLevel,
        &g_pImmediateContext
    );
    if (FAILED(hr)) return hr;

    // 3. 렌더타겟 뷰 생성
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr))  
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))  
        return hr;

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

    // 4. 뷰포트 설정
    D3D11_VIEWPORT vp = {};
    vp.Width = 800;
    vp.Height = 600;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports(1, &vp);

    // 5. 정점 버퍼
    SimpleVertex vertices[] =
    {
        {XMFLOAT3(0.0f, 0.5f, 0.0f)},
        {XMFLOAT3(0.5f, -0.5f, 0.0f)},
        {XMFLOAT3(-0.5f, -0.5f, 0.0f)},
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &g_pVertexBuffer);
    if (FAILED(hr)) 
        return hr;

    // 6. 인덱스 버퍼
    WORD indices[] = { 0, 1, 2 };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 3;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    initData.pSysMem = indices;
    hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &g_pIndexBuffer);
    if (FAILED(hr)) 
        return hr;

    // 7. 셰이더 컴파일
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    shaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    hr = D3DCompileFromFile(L"C:/Users/SeongYoon/source/repos/2025_Graphics/Source/Library/VS.hlsl", nullptr, nullptr, "VS", "vs_5_0", shaderFlags, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) return hr;
    hr = g_pd3dDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &g_pVertexShader);
    if (FAILED(hr)) return hr;
    
    hr = D3DCompileFromFile(L"C:/Users/SeongYoon/source/repos/2025_Graphics/Source/Library/VS.hlsl", nullptr, nullptr, "PS", "ps_5_0", shaderFlags, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) return hr;
    hr = g_pd3dDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &g_pPixelShader);
    if (FAILED(hr)) return hr;

    // 8. Input Layout 생성
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    hr = g_pd3dDevice->CreateInputLayout(layout, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &g_pVertexLayout);
    vsBlob->Release();
    psBlob->Release();
    if (FAILED(hr)) return hr;

    return S_OK;
}







void Render()
{
    
	float clearColor[4] = { 0.2f, 0.2f, 0.4f, 1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 셰이더 바인딩
	g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);

	g_pImmediateContext->DrawIndexed(3, 0, 0);

	g_pSwapChain->Present(0, 0);
}








LRESULT CALLBACK WindowProc(
	_In_ HWND hWnd,
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM IParam
) // 윈도우 메시지를 관리하는 함수 정의
{
	switch (uMsg)
	{
	case WM_SIZE: // window resizing
		break;

	case WM_CLOSE:
		if (MessageBox(hWnd,
			L"진짜 종료 하시겠습니까?",
			L"Game Graphics Programming",
			MB_OKCANCEL) == IDOK)
		{
			DestroyWindow(hWnd);
		}

		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, IParam);
	}


}



HRESULT InitWindow(_In_ HINSTANCE hInstance, _In_ INT nCmdShow)
{


	WNDCLASSEX wcex =
	{
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = WindowProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = LoadIcon(hInstance, IDI_APPLICATION),
		.hCursor = LoadCursor(nullptr, IDC_ARROW),
		.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
		.lpszMenuName = nullptr,
		.lpszClassName = g_pszWindowClassName,
		.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION)
	};

	if (!RegisterClassEx(&wcex)) // WndCLASSEX를 RegisterClassEx를 이용해 저장 -> 코드는 Error handling
	{
		DWORD dwError = GetLastError();

		MessageBox(
			nullptr,
			L"Call to CreateWindow failed!",
			L"Game Graphics Programming",
			NULL
		);

		if (dwError != ERROR_CLASS_ALREADY_EXISTS)
		{
			return HRESULT_FROM_WIN32(dwError);
		}

		return E_FAIL;
	}

	g_hInstance = hInstance;
	RECT rc = { 0,0,800,600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	g_hWnd = CreateWindow(
		g_pszWindowClassName,
		g_pszWindowName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		nullptr, nullptr, hInstance, nullptr
	); // CreateWindow() 함수로 window 생성 -> g_hWnd 변수에 handle 값 저장하기
	if (!g_hWnd)
	{
		DWORD dwError = GetLastError();

		MessageBox(
			nullptr,
			L"Call to CreateWindow failed!",
			L"Game Graphics Programming",
			NULL
		);

		if (dwError != ERROR_CLASS_ALREADY_EXISTS)
		{
			return HRESULT_FROM_WIN32(dwError);
		}

		return E_FAIL;
	} // handle에 대한 에러 확인

	ShowWindow(g_hWnd, nCmdShow); // 에러 검사 이후 윈도우 보이기

	return S_OK;
}


