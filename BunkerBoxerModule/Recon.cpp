#include "Recon.h"



Recon::Recon()
{
	designatedScout = nullptr;
	active = 0;
	endMission = 0;
	failures = 0;
}


Recon::~Recon()
{
}

void Recon::setScoutingMission(std::vector<BWAPI::TilePosition> locations)
{
	for (auto loc : locations)
	{
		BWAPI::Broodwar->sendText("Recon Report: Scouting Location: %d, %d", loc.x, loc.y);
	}
	BWAPI::Broodwar->sendText("End of scouting locations");
	this->scoutLocations = locations;
}
void Recon::beginScouting(BWAPI::Unit designatedScout)
{
	this->designatedScout = designatedScout; 
	active = true;
	BWAPI::Broodwar->sendText("Scout mission currently active!");
}
BWAPI::Unit Recon::endScouting()
{
	active = false; 
	BWAPI::Unit copy = designatedScout;
	designatedScout = nullptr;
	return copy;
}
void Recon::update()
{
	if (designatedScout != nullptr && designatedScout->exists() && active && scoutLocations.size() >= 1)
	{
		//BWAPI::Broodwar->sendText("RECON UPDATE : SHOULD BE MOVING NAO"); //debugging
		if (!designatedScout->move((BWAPI::Position)scoutLocations[0])) // Simple cast can convert in BWAPI
		{
			// If the call fails, then print the last error message
			BWAPI::Broodwar << "Recon Report: Unit moving error! " << std::endl;
			BWAPI::Broodwar << "Unit attempting to move from " << designatedScout->getTilePosition().x << "," << designatedScout->getTilePosition().y 
				<< " to " << scoutLocations[0].x << "," << scoutLocations[0].y << std::endl;
			BWAPI::Broodwar << BWAPI::Broodwar->getLastError() << std::endl;
		}

		if (scoutLocations[0].getApproxDistance(designatedScout->getTilePosition()) < 6)
		{
			BWAPI::Broodwar->sendText("Recon Report: Location has been scouted!");

			scoutLocations.erase(begin(scoutLocations));
		}
	}
	else if (scoutLocations.size() == 0)
	{
		//BWAPI::Broodwar->sendText("Recon Report: Scouting Mission Completed.");
		//endScouting();
	}
	else if (designatedScout == nullptr && active)
	{
		active = false;
	}

}

bool Recon::isScouting()
{
	//BWAPI::Broodwar->sendText("We are currently scouting (active): %b", active);
	return active;
}

bool Recon::hasDesignatedUnits()
{
	return !(designatedScout == nullptr);
}
