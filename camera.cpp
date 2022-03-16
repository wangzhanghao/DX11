#include <DirectXMath.h>
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "scene.h"

#include "input.h"

D3DXVECTOR3 m_RotationOld;
void Camera::Init()
{
	m_Position = D3DXVECTOR3(0.0f, 2.0f, -5.0f);
	m_Target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_Forward = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_Right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_RotationOld = m_Rotation;
	//m_Quaternion = new D3DXQUATERNION();
	//D3DXQuaternionIdentity(m_Quaternion);
}

void Camera::Uninit()
{
}

void Camera::Update()
{
	Scene* scene = Manager::GetScene();
	//D3DXVECTOR3 forward = scene->GetGameObject<Camera>(0)->GetForward();
	m_Forward = m_Target - m_Position;
	m_Forward.y = 0.0f;
	D3DXVec3Normalize(&m_Forward, &m_Forward);
	D3DXVec3Cross(&m_Right, &m_Up, &m_Forward);

	m_Position += m_Forward / 10.0f;
	m_Target += m_Forward / 10.0f;

	m_Rotation.y = 0.0f;
	if (Input::GetKeyPress('W'))
	{
		m_Position += m_Forward / 20.0f;
		m_Target += m_Forward / 20.0f;
	}
	if (Input::GetKeyPress('S'))
	{
		m_Position -= m_Forward / 20.0f;
		m_Target -= m_Forward / 20.0f;
	}
	if (Input::GetKeyPress('A'))
	{
		m_Position -= m_Right / 10.0f;
		m_Target -= m_Right / 10.0f;
	}
	if (Input::GetKeyPress('D'))
	{
		m_Position += m_Right / 10.0f;
		m_Target += m_Right / 10.0f;
	}

	if (Input::GetKeyPress('Q'))
	{
		m_Rotation.y += -0.01f;
		//D3DXQUATERNION quat;
		//D3DXVECTOR3 axis = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		//D3DXQuaternionRotationAxis(&quat, &axis, -0.05f);
		//*m_Quaternion *= quat;
	}
	if (Input::GetKeyPress('E'))
	{
		m_Rotation.y += 0.01f;
		/*D3DXQUATERNION quat;
		D3DXVECTOR3 axis = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		D3DXQuaternionRotationAxis(&quat, &axis, 0.05f);
		*m_Quaternion *= quat;*/
	}
	if (Input::GetKeyPress('R'))
	{
		m_Position.y += 0.1f;
	}
	if (Input::GetKeyPress('F'))
	{
		m_Position.y -= 0.1f;
	}
}

void Camera::Draw()
{
	D3DXMATRIX rot, trans1, trans2;
	if (m_Rotation.y != 0.0f)
	{
		D3DXMatrixTranslation(&trans1, -m_Target.x, -m_Target.y, -m_Target.z);
		//D3DXMatrixRotationY(&rot, m_Rotation.y);
		D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
		D3DXMatrixTranslation(&trans2, m_Target.x, m_Target.y, m_Target.z);
		rot = trans1 * rot* trans2;
		D3DXVec3TransformCoord(&m_Position, &m_Position, &rot);
		//D3DXVec3TransformNormal(&m_Position, &m_Position, &rot);
		m_RotationOld = m_Rotation;
	}
	CAMERA camera;
	ZeroMemory(&camera, sizeof(camera));
	camera.Position.x = m_Position.x;
	camera.Position.y = m_Position.y;
	camera.Position.z = m_Position.z;
	camera.Position.w = 1.0f;
	Renderer::SetCamera(camera);

	//ビューマトリクス設定
	D3DXMatrixLookAtLH(&m_viewMatrix, &m_Position, &m_Target, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	Renderer::SetViewMatrix(&m_viewMatrix);
	//プロジェクションマトリクス設定
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, 1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);

	Renderer::SetProjectionMatrix(&m_projectionMatrix);
}

bool Camera::CheckView(D3DXVECTOR3 Position)
{
	D3DXMATRIX vp, invvp;


	D3DXMATRIX viewMatrix;
	D3DXMatrixLookAtLH(&viewMatrix, &((m_Target - m_Position)*4.0f / 5.0f), &m_Target, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	vp = m_viewMatrix * m_projectionMatrix;
	D3DXMatrixInverse(&invvp, NULL, &vp);

	D3DXVECTOR3 vpos[4];
	D3DXVECTOR3 wpos[4];

	vpos[0] = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
	vpos[1] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	vpos[2] = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
	vpos[3] = D3DXVECTOR3(1.0f, -1.0f, 1.0f);

	D3DXVec3TransformCoord(&wpos[0], &vpos[0], &invvp);
	D3DXVec3TransformCoord(&wpos[1], &vpos[1], &invvp);
	D3DXVec3TransformCoord(&wpos[2], &vpos[2], &invvp);
	D3DXVec3TransformCoord(&wpos[3], &vpos[3], &invvp);

	D3DXVECTOR3 v, v1, v2, n;
	v = Position - m_Position;
	v1 = wpos[0] - m_Position;
	v2 = wpos[2] - m_Position;
	D3DXVec3Cross(&n, &v1, &v2);

	if (D3DXVec3Dot(&n, &v) < 0.0f)
		return false;

	v1 = wpos[1] - m_Position;
	v2 = wpos[0] - m_Position;
	D3DXVec3Cross(&n, &v1, &v2);

	if (D3DXVec3Dot(&n, &v) < 0.0f)
		return false;

	v1 = wpos[3] - m_Position;
	v2 = wpos[1] - m_Position;
	D3DXVec3Cross(&n, &v1, &v2);

	if (D3DXVec3Dot(&n, &v) < 0.0f)
		return false;

	v1 = wpos[2] - m_Position;
	v2 = wpos[3] - m_Position;
	D3DXVec3Cross(&n, &v1, &v2);

	if (D3DXVec3Dot(&n, &v) < 0.0f)
		return false;

	return true;
}
