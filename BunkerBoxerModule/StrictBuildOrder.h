#pragma once

#include "Common.h"
#include "MetaType.h"

struct StrictBuildOrderItem 
{
	MetaType metaType; 
	int supply; 
	bool built; 

	StrictBuildOrderItem(MetaType m, int s, bool b) : metaType(m), supply(s), built(b) {}
};

class StrictBuildOrder
{
	std::vector<StrictBuildOrderItem> list;
	bool active;
	
public:
	StrictBuildOrder();
	~StrictBuildOrder();

	void AddItem(MetaType type, int supply);

	void setBuilt(MetaType m);
	void setDestroyed(MetaType m);
	void ResetBuildOrderToBefore(MetaType m);
	void setActive(bool act);

	MetaType getNextItem();
	MetaType getItemBySupply(int supp);
};

