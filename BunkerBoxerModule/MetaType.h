/*
This file is taken from UAlbertaBot by David Churchill. See: https://code.google.com/p/ualbertabot/
Distributed under GPL v3, see LICENSE for details.
*/

#pragma once

#include "Common.h"

struct MetaType {

	enum type_enum { Unit, Tech, Upgrade, Command, Default };
	type_enum type;

	BWAPI::UnitCommandType commandType;
	BWAPI::UnitType unitType;
	BWAPI::TechType techType;
	BWAPI::UpgradeType upgradeType;

	MetaType() : type(MetaType::Default) {}
	MetaType(BWAPI::UnitType t) : unitType(t), type(MetaType::Unit) {}
	MetaType(BWAPI::TechType t) : techType(t), type(MetaType::Tech) {}
	MetaType(BWAPI::UpgradeType t) : upgradeType(t), type(MetaType::Upgrade) {}
	MetaType(BWAPI::UnitCommandType t) : commandType(t), type(MetaType::Command) {}

	bool isUnit() { return type == Unit; }
	bool isTech() { return type == Tech; }
	bool isUpgrade() { return type == Upgrade; }
	bool isCommand() { return type == Command; }
	bool isBuilding() { return type == Unit && unitType.isBuilding(); }
	bool isRefinery() { return isBuilding() && unitType.isRefinery(); }

	int supplyRequired()
	{
		if (isUnit())
		{
			return unitType.supplyRequired();
		}
		else
		{
			return 0;
		}
	}

	int mineralPrice()
	{
		return isUnit() ? unitType.mineralPrice() : (isTech() ? techType.mineralPrice() : upgradeType.mineralPrice());
	}

	int gasPrice()
	{
		return isUnit() ? unitType.gasPrice() : (isTech() ? techType.gasPrice() : upgradeType.gasPrice());
	}

	BWAPI::UnitType whatBuilds()
	{
		return isUnit() ? unitType.whatBuilds().first : (isTech() ? techType.whatResearches() : upgradeType.whatUpgrades());
	}

	std::string getName()
	{
		if (isUnit())
		{
			return unitType.getName();
		}
		else if (isTech())
		{
			return techType.getName();
		}
		else if (isUpgrade())
		{
			return upgradeType.getName();
		}
		else if (isCommand())
		{
			return commandType.getName();
		}
		else
		{
			assert(false);
			return "LOL";
		}
	}
};