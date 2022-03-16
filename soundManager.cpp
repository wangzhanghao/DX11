#include "main.h"
#include"soundManager.h"

void SoundManager::Load(int type)
{
	Audio* sound = new Audio();
	switch (type)
	{
	case GAMEBGM:
		sound->Load("asset\\audio\\bgm.wav");
		break;
	case BULLETSE:
		sound->Load("asset\\audio\\missile.wav");
		break;
	case EXPLOSIONSE:
		sound->Load("asset\\audio\\bgm.wav");
		break;
	default:
		break;
	}
	v_Audio.push_back(sound);

}

void SoundManager::Unload(int type)
{
	v_Audio[type]->Uninit();
}

Audio* SoundManager::GetSoundlP(int type)
{
	return v_Audio[type];
}