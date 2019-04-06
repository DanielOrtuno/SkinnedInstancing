#pragma once

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif

#ifdef MULTI_THREADING
#include <thread>
#include <mutex>
#endif // MULTI_THREADING

class ISystem
{
public:

	ISystem();
	virtual~ISystem() = 0;
};


