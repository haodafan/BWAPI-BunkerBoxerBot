#include "Recon.h"



Recon::Recon()
{
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
	return designatedScout;
}
void Recon::update()
{
	if (designatedScout && designatedScout->exists() && active && scoutLocations.size() >= 1)
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
	else if (designatedScout && active)
	{
		active = false;
	}

}
