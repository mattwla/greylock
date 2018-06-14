#include "cushion.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/action.hpp"
#include "../subbrains/subbrain.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/statusmanager.hpp"
#include <iostream>

SmartEntityCushionTemplate::SmartEntityCushionTemplate()
{
	mIngameIDs.push_back("furn_de_cushion_square_01");
	mIngameIDs.push_back("gl_sitting_stump");
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
	MWBase::GOAPStatus output(MWBase::VITALS, "stress", -1);
	MWBase::GOAPStatus input(MWBase::STATUS_VOID, "void", 0);


	std::shared_ptr<MWBase::GOAPNodeData> node(new MWBase::GOAPNodeData(input, output, 0, mPtr.getCellRef().getRefNum(), 1, "cusion node"));
	

	mGOAPNodeData.push_back(node);
	ensureStatusManager();
	mStatusManager->giveStatus(MWBase::IsFlammable);


	
}


SmartEntityCushionInstance::SmartEntityCushionInstance(std::string refid, ESM::RefNum refnum, int pings)
{
	std::cout << "new cushion" << std::endl;
	mPingCount = pings;
	mRefId = refid;
	mStatusList.push_back("comfortable");


	MWBase::GOAPStatus output(MWBase::VITALS, "stress", -1);
	MWBase::GOAPStatus input(MWBase::STATUS_VOID, "void", 0);
	std::shared_ptr<MWBase::GOAPNodeData> node(new MWBase::GOAPNodeData(input, output, 0, refnum, 1, "cusion node"));


	mGOAPNodeData.push_back(node);

	ensureStatusManager();
	mStatusManager->giveStatus(MWBase::IsFlammable);
}

std::string SmartEntityCushionInstance::getSaveString()
{
	return std::to_string(mPingCount);
}

bool SmartEntityCushionInstance::isAvailableForUse()
{
	if (mCurrentUserCount != 0)
		return false;
	else
		return true;
}

MWBase::BehaviorObject * SmartEntityCushionInstance::useWorldInstance(MWBase::Life * user)
{
	std::cout << "I was requested to be used" << std::endl;
	mCurrentUserCount = 1;
	MWBase::BehaviorObject * bo = new BOCushionSit(1);
	bo->setOwner(user);
	bo->setTarget(this);
	//new BOCushionSit(user);
	
	return bo;
}

BOCushionSit::BOCushionSit(int intensity)
{

}

void BOCushionSit::getDebugInfo()
{
	
}

MWBase::BOReturn BOCushionSit::update(float time, MWWorld::Ptr ownerptr)
{
	if (mStopRequested)
	{
		std::cout << "stop requested condition entered in sit BO" << std::endl;
		mSEITarget->mCurrentUserCount = 0; //direct hack not good mwx fix me
		return MWBase::STOPPED;
	}

	
	MWWorld::Ptr marker = mSEITarget->getPtr(); //what if I don't have ptr... btw I don't think I do. until I am on scene... so ok for now?
	MWWorld::Ptr npc = mOwnerLife->mPtr;
	ESM::Position markerPos = marker.getRefData().getPosition();
	MWBase::Environment::get().getWorld()->rotateObject(npc, 0, 0, markerPos.rot[2]); //face direction of zoneslot
	
	auto seq = npc.getClass().getCreatureStats(npc).getAiSequence();


	
	seq.clear();
	MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(npc, "sitground", 0, 1);
	
	
	
	
	return MWBase::IN_PROGRESS;
}

MWBase::BOReturn BOCushionSit::start()
{
	std::cout << "start was called " << std::endl;
	float zoffset = 100.0;
	MWWorld::Ptr marker = mSEITarget->getPtr();
	MWWorld::Ptr npc = mOwnerLife->mPtr;
	ESM::Position markerPos = marker.getRefData().getPosition();
	MWBase::Environment::get().getWorld()->moveObject(npc, markerPos.pos[0], markerPos.pos[1], markerPos.pos[2] + zoffset);
	

	return MWBase::IN_PROGRESS;
}

bool BOCushionSit::stop()
{
	std::cout << "stop requested function call in cushionsitbo" << std::endl;
	mStopRequested = true;
	//flag it is time to get up.


	//let cushion SEI know we are no longer in use.

	//assume always can stop for now.
	return true;
}
