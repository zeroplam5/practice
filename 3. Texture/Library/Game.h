#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;


// SimpleVertex ����ü (���� ����)
struct SimpleVertex {
    XMFLOAT3 Pos;
    XMFLOAT2 Material;
};

// ConstantBuffer ����ü (���̴��� �ѱ� World/View/Projection ���)
struct CBWorld { XMMATRIX World; };
struct CBView { XMMATRIX View; };
struct CBProjection { XMMATRIX Projection; };

extern DirectX::XMMATRIX g_worldMatrix;
extern DirectX::XMMATRIX g_viewMatrix;
extern DirectX::XMMATRIX g_projectionMatrix;


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

// Texture ����
extern ID3D11ShaderResourceView* g_pTextureRV;
extern ID3D11SamplerState* g_pSamplerLinear;

// CleanupDeivice �Լ�
void CleanupDevice();


HRESULT InitWindow(HINSTANCE hInstance, INT nCmdShow);
HRESULT InitDevice();
void Render();