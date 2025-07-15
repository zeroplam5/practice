
#pragma once

#include "Renderer/Model.h"

enum class CharacterDirection
{
    Forward,
    Backward,
    Left,
    Right
};

class Character : public Model
{
public:
    UINT GetNumVertices() const override;
    UINT GetNumIndices() const override;

    CharacterDirection m_currentDirection;
    XMVECTOR m_targetPosition;

    Character(_In_ const std::filesystem::path& filePath);

    void Update(_In_ FLOAT deltaTime) override;

    void Move(XMVECTOR direction, FLOAT speed);
    void RotateYInObjectCoordinate(float angle, XMVECTOR objectOffset);
    
    void Render(ID3D11DeviceContext* context) override;
};
