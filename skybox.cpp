#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "skybox.h"

void Skybox::Init()
{
	m_Model = new Model();
	m_Model->Load("asset\\model\\skybox\\skybox.obj");

	m_Position = D3DXVECTOR3(0.0f, -200.0f, 0.0f);
	m_Rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Scale = D3DXVECTOR3(80.0f, 80.0f, 80.0f);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "skyboxVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "skyboxPS.cso");
}

void Skybox::Uninit()
{
	m_Model->Unload();
	delete[] m_Model;

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void Skybox::Update()
{
}

void Skybox::Draw()
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

	m_Model->Draw();
}