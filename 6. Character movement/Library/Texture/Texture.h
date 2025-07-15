#pragma once

#include "Common.h"

class Texture
{
public:
    Texture(_In_ const std::filesystem::path& filePath)
        : m_filePath(filePath)
    {}

    HRESULT Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext);

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetTextureResourceView()
    {
        return m_textureRV;
    }

    Microsoft::WRL::ComPtr<ID3D11SamplerState>& GetSamplerState()
    {
        return m_samplerLinear;
    }

private:
    std::filesystem::path m_filePath;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureRV;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerLinear;
};