
#include "Character.h"
#include <DirectXMath.h>
#include <Renderer/Model.h>

Character::Character(_In_ const std::filesystem::path& filePath)
    : Model(filePath), m_currentDirection(CharacterDirection::Forward)
{
    m_targetPosition = m_currentPosition;
}

UINT Character::GetNumVertices() const
{
    return static_cast<UINT>(m_aVertices.size());
}

UINT Character::GetNumIndices() const
{
    return static_cast<UINT>(m_aIndices.size());
}

void Character::Update(_In_ FLOAT deltaTime)
{
    XMVECTOR diff = XMVectorSubtract(m_targetPosition, m_currentPosition);
    Translate(diff);
}

void Character::Move(XMVECTOR direction, FLOAT speed)
{
    m_targetPosition = XMVectorAdd(m_currentPosition, XMVectorScale(direction, speed));
}

void Character::RotateYInObjectCoordinate(float angle, XMVECTOR objectOffset)
{
    Translate(XMVectorNegate(objectOffset));
    RotateY(angle);
    Translate(objectOffset);
}

void Character::Render(ID3D11DeviceContext* context)
{
	Model::Render(context);
	// Additional rendering logic for the character can be added here if needed
}