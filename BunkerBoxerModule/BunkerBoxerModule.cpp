#include "BunkerBoxerModule.h"
#include <iostream>


using namespace BWAPI;
using namespace Filter;

void BunkerBoxerModule::onStart()
{
  // Hello World!
  Broodwar->sendText("Hello world!");

  // Print the map name.
  // BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
  Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

  // Enable the UserInput flag, which allows us to control the bot and type messages.
  Broodwar->enableFlag(Flag::UserInput);

  // Uncomment the following line and the bot will know about everything through the fog of war (cheat).
  //Broodwar->enableFlag(Flag::CompleteMapInformation);

  // Set the command optimization level so that common commands can be grouped
  // and reduce the bot's APM (Actions Per Minute).
  Broodwar->setCommandOptimizationLevel(2);

  // Check if this is a replay
  if ( Broodwar->isReplay() )
  {

    // Announce the players in the replay
    Broodwar << "The following players are in this replay:" << std::endl;
    
    // Iterate all the players in the game using a std:: iterator
    Playerset players = Broodwar->getPlayers();
    for(auto p : players)
    {
      // Only print the player if they are not an observer
      if ( !p->isObserver() )
        Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
    }

  }
  else // if this is not a replay
  {
    // Retrieve you and your enemy's races. enemy() will just return the first enemy.
    // If you wish to deal with multiple enemies then you must use enemies().
    if ( Broodwar->enemy() ) // First make sure there is an enemy
      Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
  }

  productionManager = ProductionManager();
  productionManager.initialize();

  intel = Intelligence(); 
  intel.initialize(); 

  recon = Recon();

  // Add the 4 SCVs to the productionManager's workers, add the command center to the productionManager's buildings
  for (auto &u : Broodwar->self()->getUnits())
  {
	  // Ignore the unit if it no longer exists
	  // Make sure to include this block when handling any Unit pointer!
	  if (!u->exists())
		  continue;

	  // Ignore the unit if it has one of the following status ailments
	  if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
		  continue;

	  // Ignore the unit if it is in one of the following states
	  if (u->isLoaded() || !u->isPowered() || u->isStuck())
		  continue;

	  // Ignore the unit if it is incomplete or busy constructing
	  if (!u->isCompleted() || u->isConstructing())
		  continue;

	  if (u->getType() == UnitTypes::Terran_SCV)
	  {
		  productionManager.addWorker(u);
	  }
	  //else if (u->getType() == UnitTypes::Terran_Command_Center)
	  //{
	//	  productionManager.addBuilding(&u);
	 // }
  }

}

void BunkerBoxerModule::onEnd(bool isWinner)
{
  // Called when the game ends
  if ( isWinner )
  {
    // Log your win here!
  }
}

void BunkerBoxerModule::onFrame()
{
  // Called once every game frame

  // Display the game frame rate as text in the upper left area of the screen
  Broodwar->drawTextScreen(200, 0,  "FPS: %d", Broodwar->getFPS() );
  Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS() );

  // Return if the game is a replay or is paused
  if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
    return;

  // Prevent spamming by only running our onFrame once every number of latency frames.
  // Latency frames are the number of frames before commands are processed.
  if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
    return;

  productionManager.update();

  // Simple scouting mission 
  int scoutTiming = 10; 

  if (Broodwar->self()->supplyUsed() >= (scoutTiming * 2 && 
	  Broodwar->self()->supplyUsed() <= (scoutTiming * 2 + 1) &&
	  !recon.isScouting()))
  {
	  // Scout the enemy base 
	  Broodwar->sendText("BB Command: Begin scouting mission!");
	  std::vector<TilePosition> baseLocation = intel.getAllEnemyStartLocations();
	  recon.setScoutingMission(baseLocation);
	  recon.beginScouting(productionManager.conscript());
  }

  if (!recon.isScouting() && recon.hasDesignatedUnits())
  {
	  // Return the scout back to its original duties 
	  BWAPI::Unit scout = recon.endScouting();
	  if (scout->getType() == BWAPI::UnitTypes::Terran_SCV)
		  productionManager.addWorker(scout);
  }

}

void BunkerBoxerModule::onSendText(std::string text)
{

  // Send the text to the game if it is not being processed.
  Broodwar->sendText("%s", text.c_str());


  // Make sure to use %s and pass the text as a parameter,
  // otherwise you may run into problems when you use the %(percent) character!

}

void BunkerBoxerModule::onReceiveText(BWAPI::Player player, std::string text)
{
  // Parse the received text
  Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void BunkerBoxerModule::onPlayerLeft(BWAPI::Player player)
{
  // Interact verbally with the other players in the game by
  // announcing that the other player has left.
  Broodwar->sendText("LMAO GET REKT %s!", player->getName().c_str());
}

void BunkerBoxerModule::onNukeDetect(BWAPI::Position target)
{

  // Check if the target is a valid position
  if ( target )
  {
    // if so, print the location of the nuclear strike target
    Broodwar << "Nuclear Launch Detected at " << target << std::endl;
  }
  else 
  {
    // Otherwise, ask other players where the nuke is!
    Broodwar->sendText("Where's the nuke?");
  }

  // You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void BunkerBoxerModule::onUnitDiscover(BWAPI::Unit unit)
{
	if (unit->getPlayer() == BWAPI::Broodwar->enemy())
		intel.addEnemy(unit);
}

void BunkerBoxerModule::onUnitEvade(BWAPI::Unit unit)
{
}

void BunkerBoxerModule::onUnitShow(BWAPI::Unit unit)
{
}

void BunkerBoxerModule::onUnitHide(BWAPI::Unit unit)
{
}

void BunkerBoxerModule::onUnitCreate(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }

  // Add this unit to an appropriate thing
  if (unit->getPlayer() != Broodwar->self())
	  return;

  if (unit->getType() == BWAPI::UnitTypes::Terran_SCV)
  {
	  //BWAPI::Unit * heap_unit = new Unit(unit); // remember to delete!!!
	  productionManager.addWorker(unit);
  }  
}

void BunkerBoxerModule::onUnitDestroy(BWAPI::Unit unit)
{
	if (unit->getPlayer() != BWAPI::Broodwar->self() && unit->getPlayer()->getType() != BWAPI::PlayerTypes::Neutral)
		intel.removeEnemy(unit);

	if (unit->getPlayer() == BWAPI::Broodwar->self())
	{
		// Maybe call some kind of delete update to check if units exist ? 
	}
}

void BunkerBoxerModule::onUnitMorph(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
}

void BunkerBoxerModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void BunkerBoxerModule::onSaveGame(std::string gameName)
{
  Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void BunkerBoxerModule::onUnitComplete(BWAPI::Unit unit)
{
	if (unit->getPlayer() != Broodwar->self())
		return;

	if (unit->getType().canProduce() && unit->getType().isBuilding())
	{
		BWAPI::Broodwar->sendText("New Production Building Completion Detected.");
		BWAPI::Broodwar->sendText(unit->getType().c_str());
		//BWAPI::Unit * heap_unit = new Unit(unit); // remember to delete!!!
		productionManager.addBuilding(unit);
	}
}
