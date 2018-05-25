#include "generic.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/action.hpp"
#include "../subbrains/subbrain.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include <iostream>

SmartEntityGenericTemplate::SmartEntityGenericTemplate()
{
}

MWBase::SmartEntityInstance * SmartEntityGenericTemplate::getInstance(const MWWorld::Ptr & ptr)
{
	return new SmartEntityGenericInstance(ptr);
}

MWBase::SmartEntityInstance * SmartEntityGenericTemplate::getInstance(std::string id, ESM::RefNum refNum)
{
	return new SmartEntityGenericInstance(id, refNum, 0);
}

MWBase::SmartEntityInstance * SmartEntityGenericTemplate::loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate)
{
	return nullptr;
}

SmartEntityGenericInstance::SmartEntityGenericInstance(const MWWorld::Ptr & ptr)
{
	mLiveCellRef = ptr.getBase();
	std::cout << "made generic smartentity" << std::endl;
	mPingCount = 0;
	mRefId = ptr.getCellRef().getRefId();
	mPtr = ptr;
}

SmartEntityGenericInstance::SmartEntityGenericInstance(std::string refid, ESM::RefNum refnum, int pings)
{
	std::cout << "new generic" << std::endl;
	mPingCount = pings;
	mRefId = refid;
}

std::string SmartEntityGenericInstance::getSaveString()
{
	return std::string();
}
