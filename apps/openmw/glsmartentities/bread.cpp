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

MWBase::SmartEntityInstance * SmartEntityBreadTemplate::loadInstance(std::string refid, int refnum, int pings)
{
	
	
	SmartEntityBreadInstance * instance = new SmartEntityBreadInstance(refid, refnum, pings);

	return instance;
}

SmartEntityBreadInstance::SmartEntityBreadInstance(const MWWorld::Ptr &ptr)
{
	std::cout << "made SmartBread" << std::endl;
	mPingCount = 0;
	mRefId = ptr.getCellRef().getRefId();
}

SmartEntityBreadInstance::SmartEntityBreadInstance(std::string refid, int refnum, int pings)
{
	mPingCount = pings;
	mRefId = refid;
}
