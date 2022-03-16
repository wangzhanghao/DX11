#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "fieldModel.h"
#include "scene.h"

void FieldModel::Init()
{
	m_Position = D3DXVECTOR3(0.0f, -220.0f, 0.0f);
	m_Rotation = D3DXVECTOR3(0.0f, 3.0f, 0.0f);
	m_Scale = D3DXVECTOR3(2.5f, 2.5f, 2.5f);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "vertexLightingVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "vertexLightingPS.cso");
}

void FieldModel::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void FieldModel::Update()
{
}

void FieldModel::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	//シェーダー設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリクス設定
	D3DXMATRIX world, scale, rot, trans;
	D3DXMatrixScaling(&scale, m_Scale.x, m_Scale.y, m_Scale.z);
	D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
	D3DXMatrixTranslation(&trans, m_Position.x, m_Position.y, m_Position.z);
	world = scale * rot*trans;
	Renderer::SetWorldMatrix(&world);

	Manager::GetScene()->GetModelManager()->GetModelP(FIELD)->Draw();
}