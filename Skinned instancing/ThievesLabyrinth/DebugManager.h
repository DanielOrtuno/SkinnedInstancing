#pragma once
#include <Windows.h>
#include <Dbghelp.h>
#include <time.h>
#include "Math.h"
#include "System.h"

#include <vector>

struct TDebugMessage;
struct TDebugLineMessage;

class CDebugManager : public ISystem
{
public:

	
	struct TDebugVertex
	{
		CMath::TVECTOR3 tPos;
		CMath::TVECTOR4 tColor;
	};

	static void DebugLog(TDebugMessage tMessage);

	static void AddDebugVertex(TDebugLineMessage tMessage);

	static void* GetDebugLineVertices();

	static int	GetDebugVertexCount();

	static void ClearDebugLines();

	static LONG WINAPI errorFunc(struct _EXCEPTION_POINTERS *pExceptionInfo);

private:

	static int			m_nLineCount;
	
	static TDebugVertex	m_tDebugLines[100024];
};

