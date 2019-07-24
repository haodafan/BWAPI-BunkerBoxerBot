#include "Intelligence.h"

using namespace BWAPI;
using namespace Filter;
//using namespace BWEM;
//using namespace BWEM::BWAPI_ext;
//using namespace BWEM::utils;
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

const std::vector<BWAPI::TilePosition> & Intelligence::getStartingLocations() const
{
	return theMap.StartingLocations();
}

const BWAPI::TilePosition & Intelligence::getEnemyStartLocation() const
{
	

	const std::vector<BWAPI::TilePosition> startLocations = getStartingLocations();
	BWAPI::TilePosition closestEnemy = TilePosition();
	for (auto location : startLocations)
	{
		BWEM::Area currentArea = Intelligence::getClosestArea(location);
		
	}
}
const BWAPI::TilePosition & Intelligence::getEnemyNaturalLocation() const
{

}

bool Intelligence::isEnemyScouted()
{

}

bool Intelligence::isEnemyNaturalScouted()
{

}

BWEM::Area Intelligence::getClosestArea(const BWAPI::TilePosition tp) const
{

}

bool Intelligence::isAreaOccupied(BWEM::Area area)
{

}

bool Intelligence::doesAreaContainBase(BWEM::Area area)
{

}

// Information Updating Functions 
void Intelligence::addEnemyArea(BWAPI::TilePosition tp, BaseType b)
{
	struct EnemyArea newEnemyArea;
	//newEnemyArea.area = getClosestArea(tp);

	newEnemyArea.center = tp;
	newEnemyArea.buildings = std::vector<BWAPI::Unit>();
	newEnemyArea.type = b;

	knownEnemyAreas.emplace_back(newEnemyArea);
}

void Intelligence::removeEnemyArea(BWAPI::TilePosition tp)
{
	for (auto enemyArea : knownEnemyAreas)
	{
		BWEM::Area * a = getArea
	}
}

void Intelligence::addEnemy(BWAPI::Unit u)
{

}

void Intelligence::removeEnemy(BWAPI::Unit u)
{

}
