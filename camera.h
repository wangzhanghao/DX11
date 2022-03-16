#pragma once
//#include<d3d11.h>
#include "gameObject.h"
class Camera:public GameObject
{
public:
	void Init();
	void Uninit();
	void Update();
	void Draw();

	D3DXMATRIX GetCamViewMatrix() { return m_viewMatrix; }
	bool CheckView(D3DXVECTOR3 Position);

private:

	D3DXMATRIX m_viewMatrix;
	D3DXVECTOR3 m_Up;
	D3DXVECTOR3 m_Forward;
	D3DXVECTOR3 m_Right;
	D3DXMATRIX m_projectionMatrix;

};