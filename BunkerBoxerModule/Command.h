#pragma once
#include "Common.h"

class Command
{
public:
	Command();
	virtual ~Command();

	virtual void Update(BWAPI::Unit);
};

