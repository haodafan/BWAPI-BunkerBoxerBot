#pragma once

#include "Common.h"
#include "BWEM-1.4.1/src/bwem.h"
//#include "BWTA.h"


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

struct EnemyBase
{
	//BWEM::Area area; 
	BWAPI::TilePosition center;
	BaseType type;
	std::vector<BWAPI::Unit> buildings;
	bool active;

	EnemyBase() = default;
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

	// Stored Enemy information 
	std::vector<BWAPI::Unit> knownEnemyUnits;
	std::vector<EnemyBase> knownEnemyBases;

	// Functions 

public:
	Intelligence();
	~Intelligence();

	void Intelligence::initialize();

	bool isEnemyScouted();
	bool isEnemyNaturalScouted();

	// Information Updating Functions 
	void addEnemyBase(BWAPI::TilePosition tp, BaseType b);
	void removeEnemyBase(BWAPI::TilePosition tp);

	void addEnemy(BWAPI::Unit u);
	void removeEnemy(BWAPI::Unit u);

	//BWTA::BaseLocation * getMainBaseLocation(BWAPI::Player player);

	BWAPI::TilePosition getMainBasePosition(BWAPI::Player player);

	std::vector<BWAPI::TilePosition> getAllEnemyStartLocations();
};

