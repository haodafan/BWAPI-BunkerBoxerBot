#include "FieldCommand.h"



FieldCommand::FieldCommand()
{
	stance = State::Passive;
	ready = false; 
	army = std::vector<BWAPI::Unit>();
	assaultLocations = std::vector<BWAPI::TilePosition>();

	// Bunker Rush Strategy
	marines = std::vector<BWAPI::Unit>();
	assaultSCVs = std::vector<BWAPI::Unit>();
	engSCVs = std::vector<BWAPI::Unit>();
	bunkers = std::vector<BWAPI::Unit>();
	lead = nullptr;
	needNewBunker = true;
}


FieldCommand::~FieldCommand()
{
}

// Called once per frame
void FieldCommand::update()
{
	// First lets get rid of shit that don't exist 
	deleteUnitsThatDontExist(army);
	deleteUnitsThatDontExist(reinforcements);
	deleteUnitsThatDontExist(marines);
	deleteUnitsThatDontExist(bunkers);
	deleteUnitsThatDontExist(assaultSCVs);
	deleteUnitsThatDontExist(engSCVs);

	if (lead == nullptr || !lead->exists())
	{
		if (reinforcements.size() > 0)
		{
			// Immediately add a reinforcement to the army if there is one
			army.emplace_back(reinforcements[0]);
			reinforcements.erase(begin(reinforcements));
		}

		for (auto u : army)
		{
			if (u->getType() == BWAPI::UnitTypes::Terran_Marine)
			{
				lead = u;
			}
		}

		// if the lead still doesn't exist, then we don't actually have an army to control
		if (lead == nullptr || !lead->exists())
			return;
	}

	if (stance == State::Travelling)
	{
			   
		for (auto u : army)
		{

			// If there are NO assault locations left, then just fucking attack anything
			if (assaultLocations.size() == 0)
			{
				// attack CLOSEST enemy
				BWAPI::Unit attackEnemy;
				for (auto &e : BWAPI::Broodwar->enemy()->getUnits())
				{
					if ((attackEnemy == nullptr) || (e->getDistance(u) < attackEnemy->getDistance(u)))
					{
						attackEnemy = e;
					}
				}
				u->attack(attackEnemy, false);
				continue;
			}

			// Detect whether or not to switch states 
			// If there is an enemy close by... 
			for (auto &e : BWAPI::Broodwar->enemy()->getUnits())
			{
				if ((e->getDistance(u) < 32)) // 8 tile positions away
				{
					BWAPI::Broodwar->sendText("Enemy detected en route!");

					if (e->getType() == BWAPI::UnitTypes::Protoss_Zealot && needNewBunker && lead)
					{
						if (u->getDistance(lead) < 100)
						{
							switchStance(State::BunkerDefense);
							return;
						}
						else
						{
							u->move(lead->getPosition());
						}
					}

					switchStance(State::Aggressive);
					break;
				}
			}

			// IF SCVs ARE ON CREEP, BUILD A BUNKER
			if (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Zerg)
			{
				if (BWAPI::Broodwar->hasCreep(u->getTilePosition()) && u->getDistance(lead) < 100)
				{
					switchStance(State::BunkerDefense);
					return;
				}
			}


			if (u != lead && u->getTilePosition().getDistance(lead->getTilePosition()) > 5)
			{
				// Don't stray too far from the lead unit
				u->move((BWAPI::Position) lead->getTilePosition());
			}
			else {
				u->attack((BWAPI::Position) assaultLocations[0]);
			}
		}

		if (BWAPI::Broodwar->getClosestUnitInRectangle(
			BWAPI::Position(assaultLocations[0]), nullptr,
			BWAPI::Position(assaultLocations[0]).x - 20, BWAPI::Position(assaultLocations[0]).y - 20,
			40, 40))
		{
			// Nothing here, move to next assault location
			assaultLocations.erase(begin(assaultLocations));
		}

	}

	if (stance == State::Aggressive)
	{
		// Attacking 
		for (auto u : army)
		{
			// NO LEAD LEASH FOR AGGRESSIVE STANCE 
			//if (lead && u->getDistance(lead) > 64) // 10 tile positions
			//{
			//	u->move(lead->getPosition());
			//}
			//else
			//{

			if (u->getTransport())
			{
				u->getTransport()->unloadAll();
			}

				BWAPI::Unit attackEnemy = nullptr;

				if (u->getType() == BWAPI::UnitTypes::Terran_SCV)
				{
					// BUILD A BUNKER IF SCVs ARE NEAR A BUILDING
					//BWAPI::Unit b = nullptr;
					for (auto enemy : BWAPI::Broodwar->enemy()->getUnits())
					{
						if (enemy->getType().isBuilding() && getDistBetweenUnits(enemy, u) < 120) // 30 tilepositions away? 
						{
							switchStance(State::BunkerDefense);
							return;
						}
					}

					// SCVs attack CLOSEST enemy
					for (auto &e : BWAPI::Broodwar->enemy()->getUnits())
					{
						if ((attackEnemy == nullptr) || (e->getDistance(u) < attackEnemy->getDistance(u)))
						{
							attackEnemy = e;
						}
					}


					// IF SCVs ARE NEAR CREEP, BUILD A BUNKER
					if (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Zerg && needNewBunker && u->getDistance(lead) < 80)
					{
						BWAPI::TilePosition curr = u->getTilePosition(); 

						for (unsigned int i = 0; i < 6; i++)
						{
							for (unsigned int j = 0; j < 4; j++)
							{
								if (BWAPI::Broodwar->hasCreep((curr.x - 3) + i, (curr.y - 2) + j))
								{
									switchStance(State::BunkerDefense);
									return;
								}
							}
						}
					}
				}

				else if (u->getType() == BWAPI::UnitTypes::Terran_Marine)
				{
					attackEnemy = GetLargestThreatFor(u); // Marines attack largest threat
					if (u == lead)
					{
						// DEBUGGING 
						// Register an event that draws the target build location
						//BWAPI::TilePosition tp = attackEnemy->getTilePosition();
						//BWAPI::UnitType ut = attackEnemy->getType();
						//BWAPI::Broodwar->registerEvent([tp, ut](BWAPI::Game*)
						//{
						//	BWAPI::Broodwar->drawBoxMap(BWAPI::Position(tp),
						//		BWAPI::Position(tp + ut.tileSize()),
						//		BWAPI::Colors::Red);
						//},
						//	nullptr,  // condition
						//	500);  // frames to run

						if (attackEnemy == nullptr || getDistBetweenUnits(attackEnemy, lead) > 120)
						{
							switchStance(State::Travelling);
							return;
						}
					}

					if ((attackEnemy->getType() == BWAPI::UnitTypes::Protoss_Zealot || attackEnemy->getType() == BWAPI::UnitTypes::Zerg_Zergling 
						|| attackEnemy->getType() == BWAPI::UnitTypes::Terran_Marine)
						&& attackEnemy->getDistance(u) < 16
						&& bunkers.size() > 0 && bunkers[0]->getDistance(u) < 400)
					{
						switchStance(State::BunkerDefense);
						return;
					}
				}
				

				// This makes sure we're not interrupting the terran marine's attacking
				if (u->getType() != BWAPI::UnitTypes::Terran_Marine && !u->isStartingAttack() && !u->isAttackFrame())
					u->attack(attackEnemy, false);
			//}
		}
	}

	if (stance == State::BunkerDefense)
	{
		if (bunkers.size() == 0 || needNewBunker)
		{
			// Make a builder
			if (engSCVs.size() == 0)
			{
				engSCVs.emplace_back(assaultSCVs[0]);
				assaultSCVs.erase(begin(assaultSCVs));
			}

			if (!engSCVs[0]->isConstructing())
				engSCVs[0]->move(engSCVs[0]->getPosition());

			// START BUILDING A BUNKER 
			// place the bunker a good distance away from the closest building
			BWAPI::Unit builder = engSCVs[0];

			BWAPI::TilePosition bunkerLocation = BWAPI::TilePosition();

			// First find the closest building to the SCV
			/*
			BWAPI::Unit closestEnemyBuilding = nullptr;
			for (auto &e : BWAPI::Broodwar->enemy()->getUnits())
			{
				if ((closestEnemyBuilding == nullptr) || (e->getDistance(builder) < closestEnemyBuilding->getDistance(builder)))
				{
					if (closestEnemyBuilding->getType().isBuilding())
						closestEnemyBuilding = e;
				}
			}

			if (closestEnemyBuilding == nullptr)
			{
				// Something is very wrong
				BWAPI::Broodwar->sendText("NO BUILDING NEARBY ... SOMETHING IS VERY WRONG!!!");
				switchStance(State::Travelling);
			}

			// Then, find the closest empty space 7 tilepositions away from that building 
			BWAPI::TilePosition relativeDistance = builder->getTilePosition() - closestEnemyBuilding->getTilePosition();

			if (relativeDistance.x > 0)
			*/

			// Find a tile position with nothing there
			bunkerLocation = builder->getTilePosition();
			int rotation = 0;
			int distance = -1;
			while (true)
			{

				if (BWAPI::Broodwar->canBuildHere(bunkerLocation, BWAPI::UnitTypes::Terran_Bunker, builder, false))
					break;

				if (rotation == 0)
					bunkerLocation.x += distance;
				else if (rotation == 1)
					bunkerLocation.y += distance;
				else if (rotation == 2)
					bunkerLocation.x -= (distance + 1);
				else if (rotation == 3)
					bunkerLocation.y -= (distance + 1);
				rotation++;
				if (rotation == 4)
				{
					rotation = 0;
					distance++;
				}

				if (distance > 10)
				{
					BWAPI::Broodwar->sendText("Cannot build anywhere!");
					break;
				}
			}
			if (bunkerLocation)
			{
				BWAPI::UnitType bunkerType = BWAPI::UnitTypes::Terran_Bunker;

				// Register an event that draws the target build location
				BWAPI::Broodwar->registerEvent([bunkerLocation, bunkerType](BWAPI::Game*)
				{
					BWAPI::Broodwar->drawBoxMap(BWAPI::Position(bunkerLocation),
						BWAPI::Position(bunkerLocation + bunkerType.tileSize()),
						BWAPI::Colors::Red);
				},
					nullptr,  // condition
					bunkerType.buildTime() + 100);  // frames to run
				builder->build(bunkerType, bunkerLocation);
				needNewBunker = false;
			}
		}
		else if (bunkers.size() == 1)
		{
			// Continue building an existing bunker
			if (!bunkers[0]->isBeingConstructed() && !attackingSCVs() && !bunkers[0]->isCompleted())
				needNewBunker = true;
			else if (!bunkers[0]->isBeingConstructed() && attackingSCVs() && !bunkers[0]->isCompleted())
			{
				if (engSCVs.size() == 0)
				{
					engSCVs.emplace_back(assaultSCVs[0]);
					assaultSCVs.erase(begin(assaultSCVs));
				}
				engSCVs[0]->rightClick(bunkers[0]);
			}

			for (auto u : army)
			{
				// Leash distance for bunker! 
				if (bunkers[0] && bunkers[0]->getDistance(u) > 12)
				{
					u->move(bunkers[0]->getPosition());
					continue;
				}

				// INCOMPLETE BUNKER : DEFENSE!!! 
				if (!bunkers[0]->isCompleted())
				{
					if (u->getType() == BWAPI::UnitTypes::Terran_Marine)
					{
						u->attack(GetLargestThreatFor(bunkers[0]), false);
					}
					else if (!u->isConstructing())
					{
						// ATTACK CLOSEST
						BWAPI::Unit attackEnemy;
						for (auto &e : BWAPI::Broodwar->enemy()->getUnits())
						{
							if ((attackEnemy == nullptr) || (e->getDistance(u) < attackEnemy->getDistance(u)))
							{
								attackEnemy = e;
							}
						}
						if (bunkers[0]->getDistance(attackEnemy) < 40)
							u->attack(attackEnemy, false);
					}
				}
				else
				{
					// Move everyone into the bunker! 
					if (u->getType() == BWAPI::UnitTypes::Terran_Marine && bunkers[0]->canLoad())
					{
						u->load(bunkers[0]);
					}

					// Repair bunker if damaged
					if (u->canRepair() && bunkers[0]->getHitPoints() < 345)
					{
						u->repair(bunkers[0]);
					}

					// If there are no nearby units, attack! 
					for (auto &e : BWAPI::Broodwar->enemy()->getUnits())
					{
						if (e->getDistance(bunkers[0]) < 120)
						{
							return;
						}
					}
					switchStance(State::Aggressive);

				}
			}

		}
	}

	for (auto it = begin(reinforcements); it != end(reinforcements); it++)
	{
		if ((*it)->getDistance(lead) < 60) // 10 tile positions away
		{
			BWAPI::Broodwar->sendText("Field Command Report: Reinforcing unit has caught up."); // Debugging
			army.emplace_back(*it);
			if ((*it)->getType() == BWAPI::UnitTypes::Terran_Marine)
				marines.emplace_back(*it);

			reinforcements.erase(it);
			break;
		}
		else
		{
			(*it)->move(lead->getPosition());
		}
	}
}

BWAPI::Unit FieldCommand::GetLargestThreatFor(BWAPI::Unit u)
{
	int largestThreatNumber = 0; 
	BWAPI::Unit largestThreat = nullptr; 
	if (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Protoss)
	{
		// KILL ZEALOTS FIRST 
		for (auto threat : BWAPI::Broodwar->enemy()->getUnits())
		{

			if (threat->exists() == false)
				continue;

			if (!largestThreat)
			{
				largestThreat = threat;
				continue;
			}

			// Logic: 
			// 1. Zealots over everything else, but only if they're in a reasonable range 
			// 2. Probes over buildings, but only if they're in reasonable range 
			// 3. Attack closest building 
			// TODO: BETTER DECISION TREE
			// TODO: Ensure units stay away from Photon Cannons 

			if (largestThreat->getType() == threat->getType())
			{
				if (getDistBetweenUnits(largestThreat, u) > getDistBetweenUnits(threat, u))
				{
					largestThreat = threat;
				}
			}
			// Current threat is zealot, largestThreat is not a zealot
			else if (threat->getType() == BWAPI::UnitTypes::Protoss_Zealot)
			{
				if (getDistBetweenUnits(threat, u) < 80)
				{
					largestThreat = threat;
				}
			}
			// Current threat is a probe, largestThreat is not a probe or a zealot 
			else if (threat->getType() == BWAPI::UnitTypes::Protoss_Probe 
					&& largestThreat->getType() != BWAPI::UnitTypes::Protoss_Zealot)
			{
				if (getDistBetweenUnits(threat, u) < 80)
				{
					largestThreat = threat;
				}
			}
			// Current threat is anything else 
			else
			{
				if (largestThreat->getType() != BWAPI::UnitTypes::Protoss_Probe &&
					largestThreat->getType() != BWAPI::UnitTypes::Protoss_Zealot)
				{
					if (getDistBetweenUnits(u, largestThreat) > getDistBetweenUnits(u, threat))
					{
						largestThreat = threat;
					}
				}
			}
		}
	}
	else if (BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Zerg)
	{
		// For Zerg it might be best to just kill the closest thing ??? 
		// TODO: Sunkens??? 
		for (auto threat : BWAPI::Broodwar->enemy()->getUnits())
		{
			if (!largestThreat)
			{
				largestThreat = threat;
				continue;
			}

			if (getDistBetweenUnits(u, largestThreat) > getDistBetweenUnits(u, threat))
				largestThreat = threat;
		}
	}
	// TODO: Terran
	else
	{
		for (auto threat : BWAPI::Broodwar->enemy()->getUnits())
		{
			if (!largestThreat)
			{
				largestThreat = threat;
				continue;
			}

			if (getDistBetweenUnits(u, largestThreat) > getDistBetweenUnits(u, threat))
				largestThreat = threat;
		}
	}

	if (largestThreat == nullptr)
	{
		BWAPI::Broodwar->sendText("Could not find largest threat ??????????????");
	}

	return largestThreat;
}

bool FieldCommand::UnitNearAssaultLocation(BWAPI::Unit u)
{
	for (auto al : assaultLocations)
	{
		if (u->getDistance((BWAPI::Position) al) < 40) // 10 tile positions away
		{
			currentTargetPos = (BWAPI::Position) al;
			return true;
		}
	}
	return false;
}

// Quick helper function i made just now
/*
void FieldCommand::deleteUnitsThatDontExist(std::vector<BWAPI::Unit> v)
{
	for (auto it = begin(v); it != end(v); it++)
	{
		if (!(*it)->exists())
		{
			v.erase(it);
		}
	}
}
*/

// Adds a unit to the army, and to their unique category
void FieldCommand::addUnit(BWAPI::Unit u)
{
	BWAPI::Broodwar->sendText("Field Command Report: Soldier Added - %s", u->getType().c_str());

	// BUNKER CODE
	if (u->getType() == BWAPI::UnitTypes::Terran_Bunker)
	{
		bunkers.emplace_back(u);
		currentBunkerPos = u->getPosition();
		return;
	}

	if (!lead)
		lead = u;

	if (stance == State::Passive)
		army.emplace_back(u);
	else
	{
		BWAPI::Broodwar->sendText("			Added as reinforcement.");
		reinforcements.emplace_back(u);
	}

	if (u->getType() == BWAPI::UnitTypes::Terran_Marine)
		marines.emplace_back(u);
	else if (u->getType() == BWAPI::UnitTypes::Terran_SCV)
		assaultSCVs.emplace_back(u);
}

// Assault functions
void FieldCommand::addAssaultLocation(BWAPI::TilePosition tp)
{
	assaultLocations.emplace_back(tp);
}

void FieldCommand::assault()
{
	if (assaultLocations.size() >= 1)
	{
		switchStance(State::Travelling);
		BWAPI::Broodwar->sendText("Field Command : Begin attack!");
	}
	else
	{
		BWAPI::Broodwar->sendText("Field Command Report: Unable to attack, no locations set");
	}
}

/// Not Implemented
void FieldCommand::continueAssault()
{
	// TBD
	//switchStance(State::Travelling)
}

/// Not Implemented
void FieldCommand::endAssault()
{
	// TBD
}

// Army Stances
void FieldCommand::switchStance(FieldCommand::State s)
{
	stance = s;

	// DEBUGGING

	if (s == State::Mustering)
	{
		BWAPI::Broodwar->sendText("NEW STANCE: MUSTERING");
	}
	else if (s == State::Aggressive)
	{
		BWAPI::Broodwar->sendText("NEW STANCE: AGGRESSIVE");
	}
	else if (s == State::Travelling)
	{
		BWAPI::Broodwar->sendText("NEW STANCE: TRAVELLING");
	}
	else if (s == State::BunkerDefense)
	{
		BWAPI::Broodwar->sendText("NEW STANCE: BUNKER DEFENSE");
	}
}

// Internal Controls
/// Not Implemented
BWAPI::Unit FieldCommand::reassign(BWAPI::Unit u)
{
	// TBD
	for (auto it = army.begin(); it != army.end(); it++)
	{
		if ((*it)->getType() == BWAPI::UnitTypes::Terran_SCV)
		{

		}
	}
	return u;
}

void FieldCommand::reassignRole(BWAPI::Unit u, std::vector<BWAPI::Unit> from, std::vector <BWAPI::Unit> to)
{
	bool found = false;
	for (auto it = from.begin(); it != from.end(); it++)
	{
		if ((*it)->getType() == u->getType())
		{
			found = true;
			from.erase(it);
		}
	}

	if (!found)
	{
		BWAPI::Broodwar->sendText("Field Command Error: Unit to be reassigned does not exist!");
		return;
	}

	to.emplace_back(u);
}

bool FieldCommand::attackingSCVs()
{
	deleteUnitsThatDontExist(engSCVs);
	deleteUnitsThatDontExist(assaultSCVs);

	if (engSCVs.size() == 0 && assaultSCVs.size() == 0)
		return false;
	else
		return true;
}

// GET FUNCTIONS
int FieldCommand::getMarineCount()
{
	return marines.size();
}

bool FieldCommand::isPassive()
{
	return (stance == State::Passive);
}