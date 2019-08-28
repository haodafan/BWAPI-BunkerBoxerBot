#pragma once
#include "Common.h"

class FieldCommand
{
	enum State {
		Passive = 0,
		Travelling = 1, 
		Mustering = 2,
		BunkerDefense = 3, 
		Aggressive = 4
	};
	std::vector<BWAPI::Unit> army;
	std::vector<BWAPI::TilePosition> assaultLocations;

	// Bunker Rush Strategy
	std::vector<BWAPI::Unit> marines; 
	std::vector<BWAPI::Unit> assaultSCVs; 
	std::vector<BWAPI::Unit> engSCVs; 
	std::vector<BWAPI::Unit> bunkers;

	std::vector<BWAPI::Unit> reinforcements;

	BWAPI::Position currentTargetPos;
	BWAPI::Position currentBunkerPos;

	BWAPI::Unit lead;

	State stance;
	bool ready;

public:
	FieldCommand();
	~FieldCommand();

	// Called once per frame
	void update(); 

	// Adds a unit to the army, and to their unique category
	void addUnit(BWAPI::Unit u);

	// Assault functions
	void addAssaultLocation(BWAPI::TilePosition tp);
	void assault();
	void continueAssault();
	void endAssault();

	// Army Stances
	void switchStance(FieldCommand::State s);

	BWAPI::Unit reassign(BWAPI::Unit u);
	void reassignRole(BWAPI::Unit u, std::vector<BWAPI::Unit> from, std::vector < BWAPI::Unit> to);

	// Helper Functions 
	int getMarineCount();
	bool isPassive();
	bool UnitNearAssaultLocation(BWAPI::Unit u);
	void deleteUnitsThatDontExist(std::vector<BWAPI::Unit> v);
};

