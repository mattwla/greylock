#include "subbrainget.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"

MWBase::SubBrainGet::SubBrainGet(MWBase::Life * life)
{
}

void MWBase::SubBrainGet::calculate(MWBase::Awareness * awareness)
{

	//search through SEs that npcs can pick up, maybe even calculate cost to get each?
}

std::string MWBase::SubBrainGet::getID()
{
	return std::string();
}
