#include "StrictBuildOrder.h"

StrictBuildOrder::StrictBuildOrder()
{
	active = true;
}


StrictBuildOrder::~StrictBuildOrder()
{
}

void StrictBuildOrder::AddItem(MetaType type, int supply)
{
	StrictBuildOrderItem item = StrictBuildOrderItem(type, supply, false);
	list.emplace_back(item); 
}

void StrictBuildOrder::setBuilt(MetaType m)
{
	// We find the first non-built item of that type 
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i].metaType.unitType == m.unitType && list[i].built == false)
		{
			list[i].built = true;
			break;
		}
	}
}

void StrictBuildOrder::setDestroyed(MetaType m)
{
	// We find the last built item of that type 
	for (int i = list.size() - 1; i >= 0; i--)
	{
		if (list[i].metaType.unitType == m.unitType && list[i].built == true)
		{
			list[i].built = false;
			break;
		}
	}
}

void StrictBuildOrder::ResetBuildOrderToBefore(MetaType m)
{
	// not yet implemented
}
void StrictBuildOrder::setActive(bool act)
{
	active = act;
}

MetaType StrictBuildOrder::getNextItem()
{
	// We find the first non-built item
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i].built == false)
		{
			return list[i].metaType;
		}
	}
	return MetaType(); // Nothing
}
MetaType StrictBuildOrder::getItemBySupply(int supp)
{
	// We find the item of that particular supply
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i].built == false && list[i].supply <= supp)
		{
			//BWAPI::Broodwar->sendText("StrictBuildOrder search success! Current supply, item supply:");
			//BWAPI::Broodwar->sendText(std::to_string(supp).c_str());
			//BWAPI::Broodwar->sendText(std::to_string(list[i].supply).c_str());
			return list[i].metaType;
		}
	}
	return MetaType(); // Nothing
}
