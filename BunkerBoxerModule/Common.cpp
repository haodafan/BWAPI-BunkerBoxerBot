#include "Common.h"

void deleteUnitsThatDontExist(std::vector<BWAPI::Unit> v)
{
	//for (auto it = begin(v); it != end(v); it++)
	//{
	//	if (!(*it) || !(*it)->exists())
	//	{
	//		it = v.erase(it);
	//	}
	//}
	v.erase(std::remove_if(v.begin(), v.end(),
			[](BWAPI::Unit u) { return !u->exists(); }), v.end());
}