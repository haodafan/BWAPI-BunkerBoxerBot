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
	this->scoutLocations = locations;
}
void Recon::beginScouting(BWAPI::Unit designatedScout)
{
	this->designatedScout = designatedScout; 
	active = true;
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
		if (!designatedScout->move((BWAPI::Position)scoutLocations[0])) // Simple cast can convert in BWAPI
		{
			// If the call fails, then print the last error message
			BWAPI::Broodwar << BWAPI::Broodwar->getLastError() << std::endl;
		}

		if (scoutLocations[0].getApproxDistance(designatedScout->getTilePosition()) < 6)
		{
			BWAPI::Broodwar->sendText("Recon Report: Location has been scouted!");

			scoutLocations.erase(begin(scoutLocations));
		}
	}
	else if (designatedScout != nullptr && active)
	{
		active = false;
	}

}

bool Recon::isScouting()
{
	return active;
}

bool Recon::hasDesignatedUnits()
{
	return !(designatedScout == nullptr);
}
