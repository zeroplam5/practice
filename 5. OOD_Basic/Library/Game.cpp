#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <windows.h>      // OutputDebugStringA ��� �� �ʿ�

#include "Common.h"       // ASSIMP_LOAD_FLAGS ���� ����

void LoadModel()
{
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile("../../Data/Tetrahedron.obj", ASSIMP_LOAD_FLAGS);

    if (!pScene || !pScene->HasMeshes()) {
        OutputDebugStringA("Failed to load model or no mesh found.\n");
        OutputDebugStringA(importer.GetErrorString());
        OutputDebugStringA("\n");
        return;
    }

    // ����� �޽���
    char szDebugMessage[256];
    sprintf_s(szDebugMessage,
        " Number of meshes: %d\n Vertices in first mesh: %d\n",
        pScene->mNumMeshes,
        pScene->mMeshes[0]->mNumVertices);

    OutputDebugStringA(szDebugMessage);
}
