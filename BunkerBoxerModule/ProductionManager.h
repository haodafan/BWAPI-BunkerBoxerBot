#pragma once
#include <BWAPI.h>
//#include <BWTA.h>

#include "Common.h"
#include "MetaType.h"
#include "StrictBuildOrder.h"

class ProductionManager
{
	// Production variables
	std::vector<const BWAPI::Unit*> productionBuildings; 

	// Worker Manager Variables
	// Currently, since our bot does not manage expansions or vespene gas, we will implement a super basic worker manager
	// directly into our production manager. 
	std::vector<const BWAPI::Unit*> allWorkers; 

	// Build Order Variables 
	bool buildOrderActive; 
	StrictBuildOrder buildOrder;
	std::vector<MetaType> saveList;
	bool saving;
	int currentLowBound;

	// Passive Variables 
	BWAPI::UnitType passiveBuildUnit; // Currently we only need to worry about passively building one unit

public:
	ProductionManager();
	~ProductionManager();

	void initialize();
	//void setBuildOrder();

	void update(); 

	void ProductionManager::addBuilding(const BWAPI::Unit* building);
	void ProductionManager::removeBuilding(const BWAPI::Unit* building);

	void ProductionManager::addWorker(const BWAPI::Unit* worker);
	void ProductionManager::removeWorker(const BWAPI::Unit* worker);

	void ProductionManager::deactivateBuildOrder();

	void ProductionManager::beginSaving(); 
	void ProductionManager::beginSpending();

	void ProductionManager::changePassiveProduction(BWAPI::UnitType u);

	const BWAPI::Unit * ProductionManager::getBuilder();

	// Extra Helper Functions 
	void ProductionManager::produceUnitFromBuilding(BWAPI::UnitType unit, BWAPI::Unit building);
	const BWAPI::Unit * ProductionManager::conscript();
};

