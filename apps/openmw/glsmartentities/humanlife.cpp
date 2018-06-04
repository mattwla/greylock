#include "humanlife.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/action.hpp"
#include "../subbrains/subbrain.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/statusmanager.hpp"
#include <iostream>


SmartEntityHumanTemplate::SmartEntityHumanTemplate()
{
	mIngameIDs.push_back("human_life");
	//special case, humans can have all sorts of ids
}

MWBase::SmartEntityInstance * SmartEntityHumanTemplate::getInstance(const MWWorld::Ptr & ptr)
{
	MWBase::SmartEntityInstance * instance = new SmartEntityHumanInstance(ptr);
	return instance;
}

MWBase::SmartEntityInstance * SmartEntityHumanTemplate::getInstance(std::string id, ESM::RefNum refNum)
{
	MWBase::SmartEntityInstance * instance = new SmartEntityHumanInstance(id, refNum, 0);
	return instance;
}

MWBase::SmartEntityInstance * SmartEntityHumanTemplate::loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate)
{
	//no load logic for now
	return nullptr;
}

SmartEntityHumanInstance::SmartEntityHumanInstance(const MWWorld::Ptr & ptr)
{
	mLiveCellRef = ptr.getBase();
	mRefId = ptr.getCellRef().getRefId();
	mPtr = ptr;
	mPingCount = 0;
	mIsLife = true;
	mStatusManager = new MWStatus::StatusManager(this);
	mStatusManager->giveStatus(MWBase::IsFlammable);
}

SmartEntityHumanInstance::SmartEntityHumanInstance(std::string refid, ESM::RefNum refnum, int pings)
{
	mRefId = refid;
	mPingCount = 0;
	mIsLife = true;
	mStatusManager = new MWStatus::StatusManager(this);
	mStatusManager->giveStatus(MWBase::IsFlammable);
}

std::string SmartEntityHumanInstance::getSaveString()
{
	return std::string();
}
