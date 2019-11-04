#pragma once
#include "Common.h"

class UnitCard
{
	std::vector<std::string> tags;
	BWAPI::Unit unit;
	
public:
	UnitCard();
	UnitCard(BWAPI::Unit unit, std::vector<std::string> tags);
	~UnitCard();

	// Tag system
	void addTag(std::string str);
	void removeTag(std::string str);
	void removeAllTags();
	bool containsTag(std::string matchtag);
	bool containsOnlyTag(std::string matchtag);
	bool containsNoneOf(std::vector<std::string> matchtags);
	bool containsOneOf(std::vector<std::string> matchtags);
	bool containsAllOf(std::vector<std::string> matchtags);

	// Get Functions
	BWAPI::Unit getUnit();
	std::vector<std::string> getTags();
};

