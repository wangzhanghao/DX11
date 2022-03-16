#include "main.h"
#include "scene.h"
#include "manager.h"
#include "renderer.h"
//#include "polygon2D.h"
#include "input.h"
#include "game.h"
#include "title.h"
#include "audio.h"
#include "soundManager.h"
Scene* Manager::m_Scene;

void Manager::Init()
{
	Renderer::Init();
	Input::Init();
	Audio::InitMaster();

	m_Scene = new Title();
	m_Scene->Init();
}

void Manager::Uninit()
{
	m_Scene->Uninit();
	delete m_Scene;

	Audio::UninitMaster();
	Input::Uninit();
	Renderer::Uninit();
}

void Manager::Update()
{
	Input::Update();

	m_Scene->Update();
}

void Manager::Draw()
{
	Renderer::Begin();

	m_Scene->Draw();

	Renderer::End();
}

