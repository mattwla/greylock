#include "cushion.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/action.hpp"
#include <iostream>

SmartEntityCushionTemplate::SmartEntityCushionTemplate()
{
	mIngameID = "furn_de_cushion_square_01";
}

MWBase::SmartEntityInstance * SmartEntityCushionTemplate::getInstance(const MWWorld::Ptr &ptr)
{
	SmartEntityCushionInstance * instance = new SmartEntityCushionInstance(ptr);
	
	return instance;
}

MWBase::SmartEntityInstance * SmartEntityCushionTemplate::getInstance(std::string id, ESM::RefNum refNum)
{
	SmartEntityCushionInstance * instance = new SmartEntityCushionInstance(id, refNum, 0);

	return instance;
}

MWBase::SmartEntityInstance * SmartEntityCushionTemplate::loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate)
{
	//instead, make pings "savestatestring"
	//needs ptr
	int pings = std::stoi(savestate);
	SmartEntityCushionInstance * instance = new SmartEntityCushionInstance(refid, refnum, pings);

	return instance;
}

SmartEntityCushionInstance::SmartEntityCushionInstance(const MWWorld::Ptr &ptr)
{
	
	mLiveCellRef = ptr.getBase();
	std::cout << "made SmartCushion" << std::endl;
	mPingCount = 0;
	mRefId = ptr.getCellRef().getRefId();
	mPtr = ptr;
	mStatusList.push_back("comfortable");
	
}


SmartEntityCushionInstance::SmartEntityCushionInstance(std::string refid, ESM::RefNum refnum, int pings)
{
	std::cout << "new cushion" << std::endl;
	mPingCount = pings;
	mRefId = refid;
	mStatusList.push_back("comfortable");
}

std::string SmartEntityCushionInstance::getSaveString()
{
	return std::to_string(mPingCount);
}

bool SmartEntityCushionInstance::use(MWBase::Life * user)
{
	//maybe have return a pointer to a BO? Not a bad idea. But for now....
	user->mVitals.mHunger = 0.0f;

	std::shared_ptr<MWWorld::Action> action = mPtr.getClass().use(mPtr);

	action->execute(user->mPtr);

	
	std::cout << "cushion used" << std::endl;
	return false;
}
