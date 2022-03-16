#include "main.h"
#include "title.h"
#include "titleField.h"
#include "manager.h"
#include "game.h"
#include "input.h"

void Title::Init()
{
	AddGameObject<TitleField>(2);
}

void Title::Uninit()
{
	Scene::Uninit();
}

void Title::Update()
{
	Scene::Update();

	if (Input::GetKeyTrigger(VK_SPACE))
	{
		Manager::SetScene<Game>();
	}
}