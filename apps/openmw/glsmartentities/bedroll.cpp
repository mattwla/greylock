#include "bedroll.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/action.hpp"
#include "../subbrains/subbrain.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include <iostream>

SmartEntityBedrollTemplate::SmartEntityBedrollTemplate()
{
	mIngameID = "se_bedroll";
}

MWBase::SmartEntityInstance * SmartEntityBedrollTemplate::getInstance(const MWWorld::Ptr &ptr)
{
	SmartEntityBedrollInstance * instance = new SmartEntityBedrollInstance(ptr);
	
	return instance;
}

MWBase::SmartEntityInstance * SmartEntityBedrollTemplate::getInstance(std::string id, ESM::RefNum refNum)
{
	SmartEntityBedrollInstance * instance = new SmartEntityBedrollInstance(id, refNum, 0);

	return instance;
}

MWBase::SmartEntityInstance * SmartEntityBedrollTemplate::loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate)
{
	//instead, make pings "savestatestring"
	//needs ptr
	int pings = std::stoi(savestate);
	SmartEntityBedrollInstance * instance = new SmartEntityBedrollInstance(refid, refnum, pings);

	return instance;
}

SmartEntityBedrollInstance::SmartEntityBedrollInstance(const MWWorld::Ptr &ptr)
{
	
	mLiveCellRef = ptr.getBase();
	std::cout << "made SmartBedroll" << std::endl;
	mCurrentUserCount = 0;
	mPingCount = 0;
	mRefId = ptr.getCellRef().getRefId();
	mPtr = ptr;
	mStatusList.push_back("comfortable");
	MWBase::GOAPStatus output(MWBase::GOAPStatus::VITALS, "sleepiness", -1);
	MWBase::GOAPStatus input(MWBase::GOAPStatus::STATUS_VOID, "void", 0);


	std::shared_ptr<MWBase::GOAPNodeData> node(new MWBase::GOAPNodeData(input, output, 0, mPtr.getCellRef().getRefNum(), 1, "cusion node"));
	

	mGOAPNodeData.push_back(node);


	
}


SmartEntityBedrollInstance::SmartEntityBedrollInstance(std::string refid, ESM::RefNum refnum, int pings)
{
	std::cout << "new Bedroll" << std::endl;
	mPingCount = pings;
	mRefId = refid;
	mStatusList.push_back("comfortable");


	MWBase::GOAPStatus output(MWBase::GOAPStatus::VITALS, "sleepiness", -1);
	MWBase::GOAPStatus input(MWBase::GOAPStatus::STATUS_VOID, "void", 0);
	std::shared_ptr<MWBase::GOAPNodeData> node(new MWBase::GOAPNodeData(input, output, 0, refnum, 1, "bedroll node"));


	mGOAPNodeData.push_back(node);
}

std::string SmartEntityBedrollInstance::getSaveString()
{
	return std::to_string(mPingCount);
}

bool SmartEntityBedrollInstance::isAvailableForUse()
{
	if (mCurrentUserCount != 0)
		return false;
	else
		return true;
}

MWBase::BehaviorObject * SmartEntityBedrollInstance::useWorldInstance(MWBase::Life * user)
{
	std::cout << "I was requested to be used" << std::endl;
	mCurrentUserCount = 1;
	MWBase::BehaviorObject * bo = new BOBedrollSleep(1);
	bo->setOwner(user);
	bo->setTarget(this);
	//new BOBedrollSit(user);
	
	return bo;
}

BOBedrollSleep::BOBedrollSleep(int valence)
{

}

void BOBedrollSleep::getDebugInfo()
{
	
}

MWBase::BOReturn BOBedrollSleep::update(float time, MWWorld::Ptr ownerptr)
{
	if (mStopRequested)
	{
		std::cout << "stop requested condition entered in bedroll BO" << std::endl;
		mSEITarget->mCurrentUserCount = 0; //direct hack not good mwx fix me
		return MWBase::COMPLETE;
	}


	MWWorld::Ptr marker = mSEITarget->getPtr(); //what if I don't have ptr... btw I don't think I do. until I am on scene... so ok for now?
	MWWorld::Ptr npc = mOwnerLife->mPtr;
	ESM::Position markerPos = marker.getRefData().getPosition();
	MWBase::Environment::get().getWorld()->rotateObject(npc, 0, 0, markerPos.rot[2]); //face direction of zoneslot

	auto seq = npc.getClass().getCreatureStats(npc).getAiSequence();


	
	seq.clear();
	MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(npc, "lay", 0, 1);
	
	
	
	
	return MWBase::IN_PROGRESS;
}

MWBase::BOReturn BOBedrollSleep::start()
{
	

	return MWBase::BOReturn();
}

bool BOBedrollSleep::stop()
{
	std::cout << "stop requested function call in Bedrollsleepbo" << std::endl;
	mStopRequested = true;
	//flag it is time to get up.


	//let Bedroll SEI know we are no longer in use.

	//assume always can stop for now.
	return true;
}
