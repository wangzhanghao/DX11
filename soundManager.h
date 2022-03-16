#pragma once
#include <vector>
#include "audio.h"
#define GAMEBGM		(0)
#define BULLETSE	(1)
#define EXPLOSIONSE	(2)

class SoundManager
{
public:
	void Load(int type);

	void Unload(int type);

	Audio* GetSoundlP(int type);

	std::vector<Audio*> GetSoundMgr() { return v_Audio; }

protected:
	std::vector<Audio*> v_Audio;
};