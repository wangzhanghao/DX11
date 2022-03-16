#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "scene.h"
#include "enemy.h"

void Enemy::Init()
{
	m_Position = D3DXVECTOR3(0.0f, 1.0f, 5.0f);
	m_Rotation = D3DXVECTOR3(0.0f, -1.5f, 0.0f);
	m_Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "vertexLightingVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "vertexLightingPS.cso");
}

void Enemy::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void Enemy::Update()
{

}

void Enemy::Draw()
{
	//視錐カリング
	Scene* scene = Manager::GetScene();
	Camera*camera = scene->GetGameObject<Camera>(0);

	if (!camera->CheckView(m_Position))
	{
		return;
	}

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

	Manager::GetScene()->GetModelManager()->GetModelP(ENEMY)->Draw();
}