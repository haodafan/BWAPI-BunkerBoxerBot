#include "UnitCard.h"

using namespace std;
using namespace BWAPI;

UnitCard::UnitCard()
{
	tags = vector<string>();
	unit = NULL;
}


UnitCard::UnitCard(BWAPI::Unit unit, std::vector<std::string> tags)
{
	this->tags = tags;
	this->unit = unit;
}


UnitCard::~UnitCard()
{
}

void UnitCard::addTag(std::string str)
{
	tags.emplace_back(str);
}
void UnitCard::removeTag(std::string str)
{
	for (auto it = begin(tags); it != end(tags); ++it)
	{
		if (*it == str)
			tags.erase(it);
	}
}
void UnitCard::removeAllTags()
{
	tags.erase(begin(tags), end(tags));
}

bool UnitCard::containsTag(std::string matchtag)
{
	for (auto it = begin(tags); it != end(tags); ++it)
	{
		if (*it == matchtag)
			return true;
	}
	return false;
}
bool UnitCard::containsOnlyTag(std::string matchtag)
{
	if (tags.size() != 1)
		return false;
	else if (tags[0] != matchtag)
		return false;
	else
		return true;
}
bool UnitCard::containsNoneOf(std::vector<std::string> matchtags)
{
	for (auto it = begin(this->tags); it != end(this->tags); ++it)
	{
		for (auto jt = begin(matchtags); jt != end(matchtags); ++jt)
		{
			if (*it == *jt)
			{
				// one of them is contained within the other
				return false;
			}
		}
	}
	return true;
}
bool UnitCard::containsOneOf(std::vector<std::string> matchtags)
{
	return !containsNoneOf(matchtags);
}

bool UnitCard::containsAllOf(std::vector<std::string> matchtags)
{
	int matches = 0;
	for (auto it = begin(this->tags); it != end(this->tags); ++it)
	{
		for (auto jt = begin(matchtags); jt != end(matchtags); ++jt)
		{
			if (*it == *jt)
			{
				// one of them is contained within the other
				matches++;
			}
		}
	}
	if (matches == matchtags.size())
		return true;
	else
		return false;
}

// Get Functions
Unit UnitCard::getUnit()
{
	return unit;
}

vector<std::string> UnitCard::getTags()
{
	return tags;
}
