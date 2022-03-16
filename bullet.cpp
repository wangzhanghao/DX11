#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "bullet.h"
#include "soundManager.h"
#include <vector>
//#include "scene.h"
#include "enemy.h"
#include "modelManager.h"

#include "game.h"
#include "audio.h"




void Bullet::Init()
{
	Scene* scene = Manager::GetScene();
	ModelManager* mgrModel = scene->GetModelManager();
	//m_Model = mgrModel->GetModelP(BULLET);
	//m_Model = ModelManager::GetModelP(BULLET);
	m_Position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_Rotation = D3DXVECTOR3(3.14f / 2, 0.0f, 0.0f);
	m_Scale = D3DXVECTOR3(0.01f, 0.01f, 0.01f);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "vertexLightingVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "vertexLightingPS.cso");


	Player *p = scene->GetGameObject<Player>(1);
	m_Target = p->returnRotation().m[2];
	_rot = p->returnRotation();
	//D3DXMatrixRotationYawPitchRoll(&_rot, p->GetRotation().y, p->GetRotation().x, p->GetRotation().z);

	scene->GetSoundManager()->GetSoundlP(BULLETSE)->Play();
}

void Bullet::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void Bullet::Update()
{
	Scene* scene = Manager::GetScene();

	Player *p = scene->GetGameObject<Player>(1);
	m_Position+= m_Target /1.0f;
	
	if (D3DXVec3Length(&(p->GetPosition() - m_Position)) < -100.0f)
	{
		SetDestroy();
		return;
	}

	std::vector<Enemy*>enemyList = scene->GetGameObjects<Enemy>(1);

	for (Enemy*enemy : enemyList)
	{
		D3DXVECTOR3 enemyPosition = enemy->GetPosition();

		D3DXVECTOR3 direction = m_Position - enemyPosition;
		float length = D3DXVec3Length(&direction);

		if (length < 2.0f)
		{
			enemy->SetDestroy();
			SetDestroy();
			D3DXVECTOR3 pos = m_Position;
			pos.y += 1.0f;
			scene->AddGameObject<Effect>(1)->SetPosition(pos);
			return;
		}
	}

}

void Bullet::Draw()
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
	world = scale * rot*_rot*trans;
	Renderer::SetWorldMatrix(&world);

	//m_Model->Draw();
	Manager::GetScene()->GetModelManager()->GetModelP(BULLET)->Draw();
}