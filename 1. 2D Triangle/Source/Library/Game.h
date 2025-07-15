#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

extern ID3D11Buffer* g_pVertexBuffer;
extern ID3D11Buffer* g_pIndexBuffer;
extern ID3D11InputLayout* g_pVertexLayout;
extern ID3D11VertexShader* g_pVertexShader;
extern ID3D11PixelShader* g_pPixelShader;
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pImmediateContext;
extern ID3D11RenderTargetView* g_pRenderTargetView;
extern IDXGISwapChain* g_pSwapChain;
extern D3D_FEATURE_LEVEL g_featureLevel;

HRESULT InitWindow(HINSTANCE hInstance, INT nCmdShow);  
HRESULT InitDevice();                                   
void Render();                                          