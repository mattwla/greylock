#include "bread.hpp"
#include <iostream>

SmartEntityBreadTemplate::SmartEntityBreadTemplate()
{
	mIngameID = "god_loaf";
}

MWBase::SmartEntityInstance * SmartEntityBreadTemplate::getInstance()
{
	SmartEntityBreadInstance * instance = new SmartEntityBreadInstance();
	
	return instance;
}

SmartEntityBreadInstance::SmartEntityBreadInstance()
{
	std::cout << "made SmartBread" << std::endl;
	mPingCount = 0;
}
