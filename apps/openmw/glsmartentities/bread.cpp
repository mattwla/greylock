#include "bread.hpp"
#include <iostream>

SmartEntityBreadTemplate::SmartEntityBreadTemplate()
{
	mIngameID = "god_loaf";
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
