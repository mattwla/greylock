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

SmartEntityBreadInstance::SmartEntityBreadInstance(const MWWorld::Ptr &ptr)
{
	std::cout << "made SmartBread" << std::endl;
	mPingCount = 0;
	mRefId = ptr.getCellRef().getRefId();
}
