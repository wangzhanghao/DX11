#include"main.h"

#include"manager.h"

#include "audio.h"
#include"game.h"
#include "result.h"
#include "input.h"

Cloud cl = {};

void Game::Init()
{
	//assert(mgr_Model);
	for (int i = 0; i < 4; i++)
	{
		mgr_Model->Load(i);
	}
	for (int i = 0; i < 2; i++)
	{
		mgr_Sound->Load(i);
	}

	/*D3DX11CreateShaderResourceViewFromFile(Renderer::GetDevice(),
		"asset/texture/20210124170012581.png",
		NULL, NULL,
		&m_PerlinNoise,
		NULL);
	assert(m_PerlinNoise);

	Renderer::GetDeviceContext()->PSSetShaderResources(1, 1, &m_PerlinNoise);*/

	AddGameObject<Camera>(0);
	AddGameObject<Skybox>(1);
	AddGameObject<FieldModel>(1);
	AddGameObject<Player>(1);

	/*AddGameObject<Enemy>(1)->SetPosition(D3DXVECTOR3(-3.0f, 1.0f, 5.0f));
	AddGameObject<Enemy>(1)->SetPosition(D3DXVECTOR3(0.0f, 1.0f, 5.0f));
	AddGameObject<Enemy>(1)->SetPosition(D3DXVECTOR3(3.0f, 1.0f, 5.0f));*/

	//AddGameObject<Polygon2D>(2);

	mgr_Sound->GetSoundlP(GAMEBGM)->Play(true);
	mgr_Sound->GetSoundMgr();

	light.Enable = true;
	light.Direction = D3DXVECTOR4(0.0f, -1.0f, -1.0f, 0.0f);
	D3DXVec4Normalize(&light.Direction, &light.Direction);
	light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	cl.speed = 0.000f;
	cl.windDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	cl.matchingstep = 0.05f;
	cl.matchingcount = 150;
}

void Game::Uninit()
{
	Scene::Uninit();
	mgr_Model->Unload();
	mgr_Sound->GetSoundlP(GAMEBGM)->Uninit();
	//m_PerlinNoise->Release();
}

void Game::Update()
{
	Scene::Update();
	/*if (GetGameObject<Enemy>(1) == NULL)
		Manager::SetScene<Result>();*/

	if (Input::GetKeyPress('I'))
	{
		light.Direction.y -= 0.01f;
		D3DXVec4Normalize(&light.Direction, &light.Direction);
	}
	if (Input::GetKeyPress('K'))
	{
		light.Direction.y += 0.01f;
		D3DXVec4Normalize(&light.Direction, &light.Direction);
	}
	light.framecount++;
	if (light.framecount == INT_MAX)
	{
		light.framecount = 0;
	}
	ImGui::SliderFloat3("LightDirection", light.Direction, -0.5f, 0.5f);
	Renderer::SetLight(light);

	ImGui::SliderFloat3("WindDirection", cl.windDir, 0.0f, 1.0f);
	ImGui::SliderFloat("WindSpeed", &cl.speed, 0.0f, 0.05f);
	ImGui::SliderFloat("MatchingStep", &cl.matchingstep, 0.0f, 0.5f);
	ImGui::SliderInt("MatchingCount", &cl.matchingcount, 0, 1000);
	Renderer::SetCloud(cl);
}