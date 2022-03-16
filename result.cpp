#include "main.h"
#include "result.h"
#include "manager.h"
#include "title.h"
#include "input.h"
#include "resultField.h"


void Result::Init()
{
	AddGameObject<ResultField>(2);
}

void Result::Uninit()
{
	Scene::Uninit();
}

void Result::Update()
{
	Scene::Update();

	if (Input::GetKeyTrigger(VK_SPACE))
	{
		Manager::SetScene<Title>();
	}
}