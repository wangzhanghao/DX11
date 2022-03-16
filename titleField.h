#pragma once
#include<d3d11.h>
#include "gameObject.h"
class TitleField :public GameObject
{
public:

	void Init();
	void Uninit();
	void Update();
	void Draw();

	void ExecParticle();
	void DrawParticle(HDC hdc);

private:
	ID3D11ShaderResourceView* t_Flight = NULL;
	ID3D11ShaderResourceView* t_Back = NULL;
	ID3D11ShaderResourceView* t_Front = NULL;
};