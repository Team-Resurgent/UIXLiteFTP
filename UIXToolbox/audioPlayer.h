#pragma once

#include "xboxInternals.h"
#include "utils.h"

typedef struct audioContainer 
{	
	uint32_t id;
	bool requestStop;
	HANDLE thread;

	~audioContainer()
	{
		utils::debugPrint("Sound %i disposing\n", id);
		CloseHandle(thread);
	}
} audioContainer;

class audioPlayer
{		
public:
	static bool init();
	static bool close();
	static uint32_t play();
	static bool stop(uint32_t key);
	static void pause(bool value);
private:
	static uint64_t WINAPI process(void* param);
};
