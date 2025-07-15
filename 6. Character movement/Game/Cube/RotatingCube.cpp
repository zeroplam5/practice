#include "Cube/RotatingCube.h"

RotatingCube::RotatingCube(_In_ const std::filesystem::path& textureFilePath)
    : BaseCube(textureFilePath)
{
}

RotatingCube::RotatingCube(_In_ const XMFLOAT4& outputColor)
    : BaseCube(outputColor)
{
}

void RotatingCube::Update(_In_ FLOAT deltaTime)
{
    static FLOAT t = 0.0f;
    t += deltaTime;

    XMMATRIX mSpin = XMMatrixRotationZ(-t);
    XMMATRIX mOrbit = XMMatrixRotationY(-t * 2.0f);
    XMMATRIX mTranslate = XMMatrixTranslation(0.0f, 0.0f, -5.0f);
    XMMATRIX mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);

    m_world = mScale * mSpin * mTranslate * mOrbit;
}