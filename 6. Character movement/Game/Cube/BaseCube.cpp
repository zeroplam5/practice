#include "Cube/BaseCube.h"

BaseCube::BaseCube(_In_ const std::filesystem::path& textureFilePath)
    : Renderable(textureFilePath)
{
}

BaseCube::BaseCube(_In_ const XMFLOAT4& outputColor)
    : Renderable(outputColor)
{
}

HRESULT BaseCube::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediateContext)
{
    BasicMeshEntry basicMeshEntry;
    basicMeshEntry.uNumIndices = NUM_INDICES;
    basicMeshEntry.uMaterialIndex = 0;

    m_aMeshes.push_back(basicMeshEntry);
    m_aMaterials.push_back(Material());

    return initialize(pDevice, pImmediateContext);
}

const SimpleVertex* BaseCube::getVertices() const
{
    return VERTICES;
}

UINT BaseCube::GetNumVertices() const
{
    return NUM_VERTICES;
}

const WORD* BaseCube::getIndices() const
{
    return INDICES;
}

UINT BaseCube::GetNumIndices() const
{
    return NUM_INDICES;
}

void BaseCube::Render(ID3D11DeviceContext* context)
{
    // 샘플러 바인딩 (경고 제거용)
    ID3D11ShaderResourceView* nullSRV = nullptr;
    ID3D11SamplerState* nullSampler = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);
    context->PSSetShaderResources(1, 1, &nullSRV);
    context->PSSetSamplers(0, 1, &nullSampler);
    context->PSSetSamplers(1, 1, &nullSampler);

    for (const auto& mesh : m_aMeshes)
    {
        context->DrawIndexed(mesh.uNumIndices, mesh.uBaseIndex, mesh.uBaseVertex);
    }
}