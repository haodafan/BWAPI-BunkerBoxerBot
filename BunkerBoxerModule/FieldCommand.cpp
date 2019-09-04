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

			// Detect whether or not to switch states 
			// If there is an enemy close by... 
			for (auto &e : BWAPI::Broodwar->enemy()->getUnits())
			{
				if ((e->getDistance(u) < 32)) // 8 tile positions away
				{
					BWAPI::Broodwar->sendText("Enemy detected en route!");
					switchStance(State::Aggressive);
					break;
				}
			}


			if (u != lead && u->getTilePosition().getDistance(lead->getTilePosition()) > 6)
			{
				// Don't stray too far from the lead unit
				u->move((BWAPI::Position) lead->getTilePosition());
			}
			else {
				u->attack((BWAPI::Position) assaultLocations[0]);
			}
		}
	}

	// A Mustering stance doesn't actually make much sense so for now I will not have it 
	/*
	if (stance == State::Mustering)
	{
		bool mustered = true; 
		for (auto u : army)
		{
			if (u != lead && u->getDistance(lead) > 48) // 12 tile positions
			{
				// Muster closer to the lead unit
				u->move((BWAPI::Position) lead->getTilePosition());
				mustered = false;
			}
		}
		if (mustered)
		{
			// Check enemy units 
			//if (UnitNearAssaultLocation(lead))
			//{
			//	switchStance(State::BunkerDefense);
			//}
			//else
			//{
				bool enemyClose;

				// GET CLOSEST ENEMY
				BWAPI::Unit closestEnemy = NULL;
				for (auto &e : BWAPI::Broodwar->enemy()->getUnits())
				{
					if ((closestEnemy == NULL) || (e->getDistance(lead) < closestEnemy->getDistance(lead)))
					{
						closestEnemy = e;
					}
				}

				// SWITCH STANCE TO ATTACKING IF CLOSE
				if (closestEnemy != NULL && closestEnemy->getDistance(lead) < 64) // 16 tile positions
				{
					switchStance(State::Aggressive);
				}
				else
				{
					switchStance(State::Travelling);
				}

			//}
		}
	}
	*/

	if (stance == State::Aggressive)
	{
		// Attack closest unit
		for (auto u : army)
		{
			if (lead && u->getDistance(lead) > 64) // 10 tile positions
			{
				u->move(lead->getPosition());
			}
			else
			{
				BWAPI::Unit closestEnemy = NULL;
				for (auto &e : BWAPI::Broodwar->enemy()->getUnits())
				{
					if ((closestEnemy == NULL) || (e->getDistance(u) < closestEnemy->getDistance(u)))
					{
						closestEnemy = e;
					}
				}

				// This makes sure we're not interrupting the terran marine's attacking
				if (u->getType() != BWAPI::UnitTypes::Terran_Marine && !u->isStartingAttack() && !u->isAttackFrame())
					u->attack(closestEnemy, false);
			}
		}
	}

	for (auto it = begin(reinforcements); it != end(reinforcements); it++)
	{
		if ((*it)->getDistance(lead) < 20) // 5 tile positions away
		{
			BWAPI::Broodwar->sendText("Field Command Report: Reinforcing unit has caught up."); // Debugging
			army.emplace_back(*it);
			if ((*it)->getType() == BWAPI::UnitTypes::Terran_Marine)
				marines.emplace_back(*it);

			reinforcements.erase(it);
		}
		else
		{
			(*it)->move(lead->getPosition());
		}
	}
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


// GET FUNCTIONS
int FieldCommand::getMarineCount()
{
	return marines.size();
}

bool FieldCommand::isPassive()
{
	return (stance == State::Passive);
}