#include "Intelligence.h"

using namespace BWAPI;
using namespace Filter;
using namespace BWEM;
using namespace BWEM::BWAPI_ext;
using namespace BWEM::utils;
namespace { auto & theMap = BWEM::Map::Instance(); }

Intelligence::Intelligence()
{
	selfStartLocation = BWAPI::TilePosition();
	closestEnemyStartLocation = BWAPI::TilePosition();
	closestEnemyNaturalLocation = BWAPI::TilePosition();

	enemyBaseScouted = false;
	enemyNaturalScouted = false;
	rushDetected = false;
}


Intelligence::~Intelligence()
{
}

void Intelligence::initialize()
{
	try 
	{
		Broodwar << "Map initialization..." << std::endl;

		theMap.Initialize();
		theMap.EnableAutomaticPathAnalysis();
		bool startingLocationsOK = theMap.FindBasesForStartingLocations();
		assert(startingLocationsOK);

		BWEM::utils::MapPrinter::Initialize(&theMap);
		BWEM::utils::printMap(theMap);      // will print the map into the file <StarCraftFolder>bwapi-data/map.bmp
		BWEM::utils::pathExample(theMap);   // add to the printed map a path between two starting locations

		Broodwar << "gg" << std::endl;

		// Continued initialization

	}
	catch (const std::exception & e)
	{
		Broodwar << "EXCEPTION: " << e.what() << std::endl;
	}
}

//const std::vector<BWAPI::TilePosition> & Intelligence::getStartingLocations() const
//{
//	return theMap.StartingLocations();
//}


bool Intelligence::isEnemyScouted()
{
	return knownEnemyBases.size() > 0;
}

bool Intelligence::isEnemyNaturalScouted()
{

}

// Information Updating Functions 
void Intelligence::addEnemyBase(BWAPI::TilePosition tp, BaseType b)
{
	struct EnemyBase newEnemyBase;
	//newEnemyArea.area = getClosestArea(tp);

	newEnemyBase.center = tp;
	newEnemyBase.buildings = std::vector<BWAPI::Unit>();
	newEnemyBase.type = b;
	newEnemyBase.active = true;

	knownEnemyBases.emplace_back(newEnemyBase);
}

void Intelligence::removeEnemyBase(BWAPI::TilePosition tp)
{
	//struct std::vector<EnemyBase>::iterator closestBase;
	double closestDist = DBL_MAX; 
	for (auto enemyBase : knownEnemyBases)
	{
		double dist = enemyBase.center.getDistance(tp); 
		if (closestDist > dist)
		{
			closestDist = dist;
			//closestBase = enemyBase;
			enemyBase.active = false;
		}
	}
	//knownEnemyBases.erase(closestBase);
}

void Intelligence::addEnemy(BWAPI::Unit u)
{
	//BWAPI::Broodwar->sendText("Intelligence Report: Enemy unit added to watch list: %s", u->getType().c_str());
	knownEnemyUnits.emplace_back(u);

	if (u->getType().isResourceDepot())
	{
		if (!isNearStartLocation(u->getTilePosition()))
		{
			BWAPI::Broodwar->sendText("Expansion Detected!");
			//BWAPI::Broodwar->sendText("Distance from start location: %d", u->getTilePosition().getApproxDistance(u->getPlayer()->getStartLocation()));
			addEnemyBase(u->getTilePosition(), BaseType::Expansion);
		}
		else 
		{
			BWAPI::Broodwar->sendText("Main Base Detected!");
			addEnemyBase(u->getTilePosition(), BaseType::Main);
		}
	}
}

void Intelligence::removeEnemy(BWAPI::Unit u)
{
	BWAPI::Broodwar->sendText("Intelligence Report: Enemy unit removed from watch list: %s", u->getType().c_str());
}


BWAPI::TilePosition Intelligence::getMainBasePosition(BWAPI::Player player)
{
	
	if (player == BWAPI::Broodwar->enemy())
	{
		for (unsigned int i = 0; i < knownEnemyBases.size(); i++)
		{
			if (knownEnemyBases[i].type == BaseType::Main)
			{
				return knownEnemyBases[i].center;
			}
		}
	}
	else if (player == BWAPI::Broodwar->self())
	{
		return BWAPI::Broodwar->self()->getStartLocation();
	}
}
BWAPI::TilePosition Intelligence::getEnemyNaturalLocation()
{
	for (int i = 0; i < knownEnemyBases.size(); i++)
	{
		if (knownEnemyBases[i].type == BaseType::Expansion) // Currently we have no way of differentiating Natural and Expansion
		{
			return knownEnemyBases[i].center;
		}
	}
}

int Intelligence::countEnemyBases()
{
	return knownEnemyBases.size();
}

std::vector<BWAPI::TilePosition> Intelligence::getAllEnemyStartLocations()
{
	std::vector<BWAPI::TilePosition> startLocations; 

	// old code
	//for (const BWAPI::Player &p : BWAPI::Broodwar->enemies())
	//{
	//	//BWAPI::Broodwar->drawTextMap((Position) p->getStartLocation(), "%c%s", Text::White, "PLAYER START LOCATION");
	//	if (p->getStartLocation() != TilePositions::None)
	//		startLocations.emplace_back(p->getStartLocation()); // Issue: Probably tile position is Unknown
	//}

	for (BWAPI::TilePosition loc : BWAPI::Broodwar->getStartLocations())
	{
		// Not our own start location, obviously :^) 
		if (loc != BWAPI::Broodwar->self()->getStartLocation())
		{
			startLocations.emplace_back(loc);
		}
	}

	return startLocations;
}

bool Intelligence::isNearStartLocation(BWAPI::TilePosition tp)
{
	bool isNear = false;
	std::vector<BWAPI::TilePosition> startLocations = getAllEnemyStartLocations();

	for (unsigned int i = 0; i < startLocations.size(); i++)
	{
		if (startLocations[i].getApproxDistance(tp) < 10)
		{
			isNear = true;
			return isNear;
		}
	}
	return isNear;
}
