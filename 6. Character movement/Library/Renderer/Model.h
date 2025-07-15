#pragma once

#include "Renderer/Renderable.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

class Model : public Renderable
{
public:
    Model(_In_ const std::filesystem::path& filePath);

    HRESULT Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext) override;
    void Update(_In_ FLOAT deltaTime) override {}
    void Render(ID3D11DeviceContext* context) override;


protected:
    const SimpleVertex* getVertices() const override;
    const WORD* getIndices() const override;

     XMVECTOR m_currentPosition = XMVectorZero();

     std::vector<SimpleVertex> m_aVertices;
     std::vector<WORD> m_aIndices;

     UINT m_uVertexOffset = 0u;
     UINT m_uIndexOffset = 0u;

private:
    std::filesystem::path m_filePath;

    

    void initFromScene(const aiScene* pScene, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void countVerticesAndIndices(const aiScene* pScene);
    void reserveSpace(const aiScene* pScene);
    void initAllMeshes(const aiScene* pScene);
    void initSingleMesh(const aiMesh* pMesh);
    void initMaterials(const aiScene* pScene, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

    std::shared_ptr<Texture> loadTexture(const aiMaterial* pMaterial, aiTextureType type, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    XMFLOAT3 loadColor(const aiMaterial* pMaterial, const char* key, UINT type, UINT idx);
};
