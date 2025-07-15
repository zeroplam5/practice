#include "Texture/Texture.h"
#include "WICTextureLoader.h"  // DirectXTK의 WICTextureLoader 사용

HRESULT Texture::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
{
    HRESULT hr = DirectX::CreateWICTextureFromFile(
        pDevice,
        pImmediateContext,
        m_filePath.c_str(),
        nullptr,
        m_textureRV.GetAddressOf()
    );

    if (FAILED(hr))
    {
        OutputDebugString(L"[Texture] Failed to load texture: ");
        OutputDebugString(m_filePath.c_str());
        OutputDebugString(L"\n");
        return hr;
    }

    // 샘플러 상태 생성
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = pDevice->CreateSamplerState(&sampDesc, m_samplerLinear.GetAddressOf());
    if (FAILED(hr))
    {
        OutputDebugString(L"[Texture] Failed to create sampler state\n");
        return hr;
    }

    return S_OK;
}