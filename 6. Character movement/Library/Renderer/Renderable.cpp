#include "Renderer/Renderable.h"

#include "Texture/DDSTextureLoader11.h"

Renderable::Renderable(_In_ const std::filesystem::path& textureFilePath)
	: m_vertexBuffer()
	, m_indexBuffer()
	, m_cbChangeEveryFrame()
	, m_textureRV()
	, m_samplerLinear()
	, m_vertexShader()
	, m_pixelShader()
	, m_textureFilePath(textureFilePath)
	, m_outputColor(XMFLOAT4(1.0f, 1.0f, 1.0f,1.0f))
	, m_world(XMMatrixIdentity())
	, m_bHasTexture(TRUE)
{
}

Renderable::Renderable(_In_ const XMFLOAT4& outputColor)
	: m_vertexBuffer()
	, m_indexBuffer()
	, m_cbChangeEveryFrame()
	, m_textureRV()
	, m_samplerLinear()
	, m_vertexShader()
	, m_pixelShader()
	, m_textureFilePath()
	, m_outputColor(outputColor)
	, m_world(XMMatrixIdentity())
	, m_bHasTexture(FALSE)
{
}

HRESULT Renderable::initialize(
	_In_ ID3D11Device* pDevice,
	_In_ ID3D11DeviceContext* pImmediateContext
)
{
	D3D11_BUFFER_DESC bd = {};
	{
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(SimpleVertex) * GetNumVertices();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
	}

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = getVertices();

	HRESULT hr = pDevice->CreateBuffer(&bd, &initData, m_vertexBuffer.GetAddressOf());

	if (FAILED(hr))
		return hr;

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * GetNumIndices();
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	initData.pSysMem = getIndices();

	hr = pDevice->CreateBuffer(&bd, &initData, m_indexBuffer.GetAddressOf());

	if (FAILED(hr))
		return hr;

	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = sizeof(CBChangeEveryFrame);
	hr = pDevice->CreateBuffer(&bd, nullptr, m_cbChangeEveryFrame.GetAddressOf());
	if (FAILED(hr))
		return hr;

	if (m_bHasTexture)
	{
		hr = CreateDDSTextureFromFile(pDevice, m_textureFilePath.filename().wstring().c_str(), nullptr, m_textureRV.GetAddressOf());
		if (FAILED(hr))
			return hr;

		D3D11_SAMPLER_DESC sampDesc = { };
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = pDevice->CreateSamplerState(&sampDesc, m_samplerLinear.GetAddressOf());
		if (FAILED(hr))
			return hr;
	}

	return S_OK;
}

void Renderable::SetVertexShader(_In_ const std::shared_ptr<VertexShader>& vertexShader)
{
	m_vertexShader = vertexShader;
}

void Renderable::SetPixelShader(_In_ const std::shared_ptr<PixelShader>& pixelShader)
{
	m_pixelShader = pixelShader;
}




ComPtr<ID3D11VertexShader>& Renderable::GetVertexShader()
{
	return m_vertexShader->GetVertexShader();
}

ComPtr<ID3D11PixelShader>& Renderable::GetPixelShader()
{
	return m_pixelShader->GetPixelShader();
}

ComPtr<ID3D11InputLayout>& Renderable::GetVertexLayout()
{
	return m_vertexShader->GetVertexLayout();
}

ComPtr<ID3D11Buffer>& Renderable::GetVertexBuffer()
{
	return m_vertexBuffer;
}

ComPtr<ID3D11Buffer>& Renderable::GetIndexBuffer()
{
	return m_indexBuffer;
}

ComPtr<ID3D11Buffer>& Renderable::GetConstantBuffer()
{
	return m_cbChangeEveryFrame;
}

const XMMATRIX& Renderable::GetWorldMatrix() const
{
	return m_world;
}

ComPtr<ID3D11ShaderResourceView>& Renderable::GetTextureResourceView()
{
	return m_textureRV;
}

ComPtr<ID3D11SamplerState>& Renderable::GetSamplerState()
{
	return m_samplerLinear;
}

const XMFLOAT4& Renderable::GetOutputColor() const
{
	return m_outputColor;
}



void Renderable::RotateX(_In_ FLOAT angle)
{
	m_world *= XMMatrixRotationX(angle);
}

void Renderable::RotateY(_In_ FLOAT angle)
{
	m_world *= XMMatrixRotationY(angle);
}

void Renderable::RotateZ(_In_ FLOAT angle)
{
	m_world *= XMMatrixRotationZ(angle);
}

void Renderable::RotateRollPitchYaw(_In_ FLOAT roll, _In_ FLOAT pitch, _In_ FLOAT yaw)
{
	m_world *= XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

void Renderable::Scale(_In_ FLOAT scaleX, _In_ FLOAT scaleY, _In_ FLOAT scaleZ)
{
	m_world *= XMMatrixScaling(scaleX, scaleY, scaleZ);
}

void Renderable::Translate(_In_ const XMVECTOR& offset)
{
	m_world *= XMMatrixTranslationFromVector(offset);
}
