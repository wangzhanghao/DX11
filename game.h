#pragma once
#include"scene.h"

class Game :public Scene
{
public:
	void Init();
	void Uninit();
	void Update();

private:
	ID3D11ShaderResourceView*	m_PerlinNoise = NULL;			//�e�N�X�`��

	LIGHT light;
};

