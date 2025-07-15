#include "Model.h"
#include "Texture/Texture.h"

Model::Model(_In_ const std::filesystem::path& filePath)
    : m_filePath(filePath)
{}

HRESULT Model::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
{
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(
        m_filePath.string(),
        aiProcess_Triangulate | aiProcess_ConvertToLeftHanded
    );

    if (!pScene)
        return E_FAIL;

    initFromScene(pScene, pDevice, pImmediateContext);
    return initialize(pDevice, pImmediateContext);
}

void Model::initFromScene(const aiScene* pScene, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    countVerticesAndIndices(pScene);
    reserveSpace(pScene);
    initAllMeshes(pScene);
    initMaterials(pScene, pDevice, pContext);
}

void Model::countVerticesAndIndices(const aiScene* pScene)
{
    UINT uVertexCount = 0, uIndexCount = 0;
    for (UINT i = 0; i < pScene->mNumMeshes; ++i)
    {
        uVertexCount += pScene->mMeshes[i]->mNumVertices;
        uIndexCount += pScene->mMeshes[i]->mNumFaces * 3;
    }
    m_aVertices.reserve(uVertexCount);
    m_aIndices.reserve(uIndexCount);
    m_aMeshes.reserve(pScene->mNumMeshes);
}

void Model::reserveSpace(const aiScene* pScene)
{
    m_aMaterials.resize(pScene->mNumMaterials);
}

void Model::initAllMeshes(const aiScene* pScene)
{
    for (UINT i = 0; i < pScene->mNumMeshes; ++i)
    {
        initSingleMesh(pScene->mMeshes[i]);
    }
}

void Model::initSingleMesh(const aiMesh* pMesh)
{
    BasicMeshEntry entry;
    entry.uBaseVertex = static_cast<UINT>(m_aVertices.size());
    entry.uBaseIndex = static_cast<UINT>(m_aIndices.size());
    entry.uNumIndices = pMesh->mNumFaces * 3;
    entry.uMaterialIndex = pMesh->mMaterialIndex;

    for (UINT i = 0; i < pMesh->mNumVertices; ++i)
    {
        SimpleVertex v = {};
        v.Position = XMFLOAT3(
            pMesh->mVertices[i].x,
            pMesh->mVertices[i].y,
            pMesh->mVertices[i].z
        );

        if (pMesh->HasNormals())
        {
            v.Normal = XMFLOAT3(
                pMesh->mNormals[i].x,
                pMesh->mNormals[i].y,
                pMesh->mNormals[i].z
            );
        }

        if (pMesh->HasTextureCoords(0))
        {
            v.TexCoord = XMFLOAT2(
                pMesh->mTextureCoords[0][i].x,
                pMesh->mTextureCoords[0][i].y
            );
        }

        m_aVertices.push_back(v);
    }

    for (UINT i = 0; i < pMesh->mNumFaces; ++i)
    {
        const aiFace& face = pMesh->mFaces[i];
        for (UINT j = 0; j < 3; ++j)
            m_aIndices.push_back(static_cast<WORD>(face.mIndices[j]));
    }

    m_aMeshes.push_back(entry);
}

void Model::initMaterials(const aiScene* pScene, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    for (UINT i = 0; i < pScene->mNumMaterials; ++i)
    {
        const aiMaterial* pMat = pScene->mMaterials[i];

        Material material = {};
        material.pDiffuse = loadTexture(pMat, aiTextureType_DIFFUSE, pDevice, pContext);
        material.pSpecular = loadTexture(pMat, aiTextureType_SPECULAR, pDevice, pContext);
        material.AmbientColor = loadColor(pMat, AI_MATKEY_COLOR_AMBIENT);
        material.DiffuseColor = loadColor(pMat, AI_MATKEY_COLOR_DIFFUSE);
        material.SpecularColor = loadColor(pMat, AI_MATKEY_COLOR_SPECULAR);

        m_aMaterials[i] = material;
    }
}

std::shared_ptr<Texture> Model::loadTexture(const aiMaterial* pMaterial, aiTextureType type, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (pMaterial->GetTextureCount(type) > 0)
    {
        aiString path;
        if (pMaterial->GetTexture(type, 0, &path) == AI_SUCCESS)
        {
            auto fullPath = m_filePath.parent_path() / path.C_Str();
            auto tex = std::make_shared<Texture>(fullPath);
            if (SUCCEEDED(tex->Initialize(pDevice, pContext)))
                return tex;
        }
    }
    return nullptr;
}

XMFLOAT3 Model::loadColor(const aiMaterial* pMaterial, const char* key, UINT type, UINT idx)
{
    aiColor3D color(0.f, 0.f, 0.f);
    if (pMaterial->Get(key, type, idx, color) == AI_SUCCESS)
    {
        return XMFLOAT3(color.r, color.g, color.b);
    }
    return XMFLOAT3(0.f, 0.f, 0.f);
}

const SimpleVertex* Model::getVertices() const
{
    return m_aVertices.data();
}

const WORD* Model::getIndices() const
{
    return m_aIndices.data();
}

void Model::Render(ID3D11DeviceContext* context)
{
    for (const auto& mesh : m_aMeshes)
    {
        const auto& material = m_aMaterials[mesh.uMaterialIndex];

        if (material.pDiffuse)
        {
            context->PSSetShaderResources(0, 1, material.pDiffuse->GetTextureResourceView().GetAddressOf());
            context->PSSetSamplers(0, 1, material.pDiffuse->GetSamplerState().GetAddressOf());
        }
        else
        {
            ID3D11ShaderResourceView* nullSRV = nullptr;
            ID3D11SamplerState* nullSampler = nullptr;
            context->PSSetShaderResources(0, 1, &nullSRV);
            context->PSSetSamplers(0, 1, &nullSampler);
        }

        if (material.pSpecular)
        {
            context->PSSetShaderResources(1, 1, material.pSpecular->GetTextureResourceView().GetAddressOf());
            context->PSSetSamplers(1, 1, material.pSpecular->GetSamplerState().GetAddressOf());
        }
		else
		{
			ID3D11ShaderResourceView* nullSRV = nullptr;
			ID3D11SamplerState* nullSampler = nullptr;
			context->PSSetShaderResources(1, 1, &nullSRV);
			context->PSSetSamplers(1, 1, &nullSampler);
		}

        context->DrawIndexed(
            mesh.uNumIndices,
            mesh.uBaseIndex,
            static_cast<INT>(mesh.uBaseVertex)
        );
    }
}
