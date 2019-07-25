#pragma once
#include "BWAPI.h"

// Reconaissance Class: 
//     Note that Reconaissance is different from Intelligence. 
//     The Recon class performs the duty of active scouting, while intelligence stores the information

// Recon Strategy: 
//     The Recon class starts off inactive. At a time designated by the BunkerBoxerModule, 
//     it will be assigned a designated scout. The Recon class will control this scout until either the 
//     scouting mission has been completed or the scout dies. 

//     There will only be one type of scouting mission in this particular ai: worker scouting. 
//       1. A worker is designated as a scout. 
//       2. The scout then travels to the enemy's natural expansion. If the enemy's location is unknown, it goes to the closest possible natural. 
//       3. The scout then travels to the chokepoint which connects the main and natural (this is done in case of wall-off). 
//       4. The scout then travels into the main and explores the whole area. 
//       5. At this point the scout's mission is completed.
class Recon
{
	enum ScoutingMission {
		Worker = 0,
		Comscan = 1, 
	};

	BWAPI::Unit designatedScout; 
	std::vector<BWAPI::TilePosition> scoutLocations;
	bool active;
	bool endMission;
	int failures;
	
public:
	void setScoutingMission(std::vector<BWAPI::TilePosition> locations);
	void beginScouting(BWAPI::Unit designatedScout);
	BWAPI::Unit endScouting();
	void update();

	Recon();
	~Recon();
};

