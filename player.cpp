#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "scene.h"

#include "input.h"

//#include "bullet.h"
//クォータニオン追加
void Player::Init()
{
	Scene* scene = Manager::GetScene();
	m_Position = D3DXVECTOR3(0.0f, 1.0f, -2.0f);
	m_Rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Scale = D3DXVECTOR3(0.2f, 0.2f, 0.2f);

	/*m_Quaternion = new D3DXQUATERNION();
	D3DXQuaternionIdentity(m_Quaternion);*/

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "vertexLightingVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "vertexLightingPS.cso");

	//playerShadow = scene->AddGameObject<Shadow>(1);
}

void Player::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void Player::Update()
{
	Scene* scene = Manager::GetScene();
	D3DXVECTOR3 cameraPosition = scene->GetGameObject<Camera>(0)->GetPosition();
	D3DXVECTOR3 cameraTarget = scene->GetGameObject<Camera>(0)->GetTarget();
	cameraTarget.y -= 1.0f;
	m_Position = cameraTarget;
	m_Target = cameraTarget - cameraPosition;
	m_Target.y = 0.0f;
	D3DXVec3Normalize(&m_Target, &m_Target);
	//D3DXQUATERNION quat;
	//D3DXVECTOR3 axis = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	/*m_Quaternion->w = cosf(m_Rotation.x / 2)*cosf(m_Rotation.y / 2)*cosf(m_Position.z / 2) + sinf(m_Rotation.x / 2)*sinf(m_Rotation.y / 2)*sinf(m_Position.z / 2);
	m_Quaternion->x = sinf(m_Rotation.x / 2)*cosf(m_Rotation.y / 2)*cosf(m_Position.z / 2) - cosf(m_Rotation.x / 2)*sinf(m_Rotation.y / 2)*sinf(m_Position.z / 2);
	m_Quaternion->y = cosf(m_Rotation.x / 2)*sinf(m_Rotation.y / 2)*cosf(m_Position.z / 2) + sinf(m_Rotation.x / 2)*cosf(m_Rotation.y / 2)*sinf(m_Position.z / 2);
	m_Quaternion->z = cosf(m_Rotation.x / 2)*cosf(m_Rotation.y / 2)*sinf(m_Position.z / 2) - sinf(m_Rotation.x / 2)*sinf(m_Rotation.y / 2)*cosf(m_Position.z / 2);*/
	//*m_Quaternion = quat;

	if (Input::GetKeyPress(VK_LEFT))
	{
		m_Rotation.y -= 0.1f;
		m_Rotation.z -= 0.1f;

		//D3DXQUATERNION quat;
		//D3DXVECTOR3 axis = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		//D3DXQuaternionRotationAxis(&quat, &axis, -0.1f);
		//*m_Quaternion *= quat;
	}
	if (Input::GetKeyPress(VK_RIGHT))
	{
		m_Rotation.y += 0.1f;
		m_Rotation.z += 0.1f;

		//D3DXQUATERNION quat;
		//D3DXVECTOR3 axis = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		//D3DXQuaternionRotationAxis(&quat, &axis, 0.1f);
		//*m_Quaternion *= quat;
	}
	
	/*if (Input::GetKeyTrigger(VK_SPACE))
	{
		Scene* scene = Manager::GetScene();
		D3DXVECTOR3 position = m_Position;
		position.y += 0.3f;
		scene->AddGameObject<Bullet>(1)->SetPosition(position);
	}*/

	D3DXVECTOR3 pos = m_Position;
	pos.y = -3.0f;
	//playerShadow->SetPosition(pos);
}

void Player::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	//シェーダー設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリクス設定
	D3DXMATRIX world, scale, trans;
	D3DXMatrixScaling(&scale, m_Scale.x, m_Scale.y, m_Scale.z);
	//D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
	//D3DXMatrixRotationQuaternion(&rot, m_Quaternion);
	D3DXVECTOR3 right;
	D3DXVec3Cross(&right, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &m_Target);
	rot._11 = right.x;
	rot._12 = right.y;
	rot._13 = right.z;
	rot._14 = 0.0f;
	rot._21 = 0.0f;
	rot._22 = 1.0f;
	rot._23 = 0.0f;
	rot._24 = 0.0f;
	rot._31 = m_Target.x;
	rot._32 = m_Target.y;
	rot._33 = m_Target.z;
	rot._34 = 0.0f;
	rot._41 = 0.0f;
	rot._42 = 0.0f;
	rot._43 = 0.0f;
	rot._44 = 1.0f;
	m_Rotation.x = asinf(rot._23);
	m_Rotation.y = acosf(rot._33 / cosf(m_Rotation.x));
	m_Rotation.z = acosf(rot._22 / cosf(m_Rotation.x));
	D3DXMatrixTranslation(&trans, m_Position.x, m_Position.y, m_Position.z);
	world = scale * rot*trans;
	Renderer::SetWorldMatrix(&world);

	Manager::GetScene()->GetModelManager()->GetModelP(PLAYER)->Draw();
}
