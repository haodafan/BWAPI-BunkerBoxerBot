#pragma once

#include "Common.h"
//#include "BWEM-1.4.1/src/bwem.h"
#include "BWTA.h"


// The Intelligence Class manages information. 
// For this strategy, it will store: 
//    - Enemy base location 
//    - Enemy natural location

// Unlike the InformationManager class from UAlbertaBot, this class is extremely simplified 
// We will make heavy use of the functions provided by the BWTA2 Library 

class Map;

enum BaseType {
	Main = 0,
	Natural = 1,
	Expansion = 2,
};

struct EnemyArea
{
	//BWEM::Area area; 
	BWAPI::TilePosition center;
	BaseType type;
	std::vector<BWAPI::Unit> buildings;

	EnemyArea() = default;
};

class Intelligence
{
	BWAPI::Player _self;
	BWAPI::Player _enemy;
	//BWEM::Map * m;

	// Stored Map information
	BWAPI::TilePosition selfStartLocation;
	BWAPI::TilePosition closestEnemyStartLocation;
	BWAPI::TilePosition closestEnemyNaturalLocation;

	bool rushDetected; 

	bool enemyBaseScouted;
	bool enemyNaturalScouted;
	int scoutTimer; // Number of frames since last 'scouting'
	//const int scoutTimerMaximum = 1000; // not implemented

	// Stored Enemy information 
	std::vector<BWAPI::Unit> knownEnemyUnits;
	std::vector<EnemyArea> knownEnemyAreas;

	// Functions 

public:
	Intelligence();
	~Intelligence();

	void Intelligence::initialize();
	//BWTA::Region * Intelligence::getEnemyBaseLocation();
	//BWTA::Region * Intelligence::getEnemyNaturalLocation();
	//const std::vector<BWAPI::TilePosition> & getStartingLocations() const;
	const BWAPI::TilePosition & getEnemyStartLocation() const;
	const BWAPI::TilePosition & getEnemyNaturalLocation() const;

	bool isEnemyScouted();
	bool isEnemyNaturalScouted();

	//BWEM::Area getClosestArea(const BWAPI::TilePosition tp) const;
	//bool isAreaOccupied(BWEM::Area area);
	//bool doesAreaContainBase(BWEM::Area area);

	// Information Updating Functions 
	void addEnemyArea(BWAPI::TilePosition tp, BaseType b);
	void removeEnemyArea(BWAPI::TilePosition tp);

	void addEnemy(BWAPI::Unit u);
	void removeEnemy(BWAPI::Unit u);
};

