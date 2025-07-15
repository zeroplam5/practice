#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;


// SimpleVertex ����ü (���� ����)
struct SimpleVertex {
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
};

// ConstantBuffer ����ü (���̴��� �ѱ� World/View/Projection ���)
struct ConstantBuffer {
    XMMATRIX World;
    XMMATRIX View;
    XMMATRIX Projection;
};


HRESULT CompileShaderFromFile(PCWSTR, PCSTR, PCSTR, ID3DBlob**);

// ConstantBuffer ����ü (���̴��� �ѱ� Light ���)
extern ID3D11Buffer* g_pVertexBuffer;
extern ID3D11Buffer* g_pIndexBuffer;
extern ID3D11InputLayout* g_pVertexLayout;

// Vertex Shader�� Pixel Shader
extern ID3D11VertexShader* g_pVertexShader;
extern ID3D11PixelShader* g_pPixelShader;

// Device Context
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pImmediateContext;


// Swap Chain
extern ID3D11RenderTargetView* g_pRenderTargetView;
extern IDXGISwapChain* g_pSwapChain;
extern D3D_FEATURE_LEVEL g_featureLevel;

// Depth Stencil View
extern ID3D11Buffer* g_pConstantBuffer;
extern XMMATRIX g_worldMatrix;
extern XMMATRIX g_viewMatrix;
extern XMMATRIX g_projectionMatrix;

// CleanupDeivice �Լ�
void CleanupDevice();


HRESULT InitWindow(HINSTANCE hInstance, INT nCmdShow);  
HRESULT InitDevice();                                   
void Render();                                          