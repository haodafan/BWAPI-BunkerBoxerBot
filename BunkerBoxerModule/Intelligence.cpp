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
	BWAPI::Broodwar->sendText("Intelligence Report: Enemy unit added to watch list: %s", u->getType().c_str());
	knownEnemyUnits.emplace_back(u);
}

void Intelligence::removeEnemy(BWAPI::Unit u)
{
	BWAPI::Broodwar->sendText("Intelligence Report: Enemy unit removed from watch list: %s", u->getType().c_str());
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
