#include "subbrainget.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"

MWBase::SubBrainGet::SubBrainGet(MWBase::Life * life)
{
	mOwnerLife = life;
}

void MWBase::SubBrainGet::calculate(MWBase::Awareness * awareness)
{
	std::vector<SensoryLink> linkstore = awareness->getSensoryLinkStore()->mCurrentSensoryLinks;
	std::vector<SensoryLink>::iterator it = linkstore.begin();
	while (it != linkstore.end())
	{
		std::cout << "Oh I see" + it->mPtr.getCellRef().getRefId() << std::endl;
		//for every item, create a getBO
		it++;
	}
	//search through SEs that npcs can pick up, maybe even calculate cost to get each?
	//only search throug new SEs?
}

std::string MWBase::SubBrainGet::getID()
{
	return std::string();
}

void MWBase::SubBrainGet::getDebugInfo()
{
}

void MWBase::BOGet::getDebugInfo()
{
}
