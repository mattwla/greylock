#include "bread.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/action.hpp"
#include <iostream>

SmartEntityBreadTemplate::SmartEntityBreadTemplate()
{
	mIngameIDs.push_back("god_loaf");
	mIngameIDs.push_back("zeatshroom");
}

MWBase::SmartEntityInstance * SmartEntityBreadTemplate::getInstance(const MWWorld::Ptr &ptr)
{
	SmartEntityBreadInstance * instance = new SmartEntityBreadInstance(ptr);
	
	return instance;
}

MWBase::SmartEntityInstance * SmartEntityBreadTemplate::getInstance(std::string id, ESM::RefNum refNum)
{
	SmartEntityBreadInstance * instance = new SmartEntityBreadInstance(id, refNum, 0);

	return instance;
}

MWBase::SmartEntityInstance * SmartEntityBreadTemplate::loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate)
{
	//instead, make pings "savestatestring"
	//needs ptr
	int pings = std::stoi(savestate);
	SmartEntityBreadInstance * instance = new SmartEntityBreadInstance(refid, refnum, pings);

	return instance;
}

SmartEntityBreadInstance::SmartEntityBreadInstance(const MWWorld::Ptr &ptr)
{
	
	mLiveCellRef = ptr.getBase();
	std::cout << "made SmartBread" << std::endl;
	mPingCount = 0;
	mRefId = ptr.getCellRef().getRefId();
	mPtr = ptr;
	mStatusList.push_back("edible");
	
}


SmartEntityBreadInstance::SmartEntityBreadInstance(std::string refid, ESM::RefNum refnum, int pings)
{
	std::cout << "new bread" << std::endl;
	mPingCount = pings;
	mRefId = refid;
	mStatusList.push_back("edible");
}

std::string SmartEntityBreadInstance::getSaveString()
{
	return std::to_string(mPingCount);
}

bool SmartEntityBreadInstance::use(MWBase::Life * user)
{
	//maybe have return a pointer to a BO? Not a bad idea. But for now....
	user->mVitals.mHunger = 0.0f;

	std::shared_ptr<MWWorld::Action> action = mPtr.getClass().use(mPtr);

	action->execute(user->mPtr);

	user->mAwareness->getSensoryLinkStore()->removeSensoryLink(mPtr.getCellRef().getRefNum());
	std::cout << "bread used" << std::endl;
	return false;
}
