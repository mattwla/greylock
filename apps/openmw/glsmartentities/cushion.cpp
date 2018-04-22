#include "cushion.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/action.hpp"
#include "../subbrains/subbrain.hpp"
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

	std::shared_ptr<MWBase::GOAPNodeData> node(new MWBase::GOAPNodeData);
	MWBase::GOAPStatus output(MWBase::GOAPStatus::VITALS, "stress", -1);
	MWBase::GOAPStatus input(MWBase::GOAPStatus::STATUS_VOID, "void", 0);
	node->mOutputs.push_back(output);
	node->mInputs.push_back(input);
	mGOAPNodeData.push_back(node);
	
	
	
	//node->mBehaviorObject = mUseSEInWorldBO;
	//node->mSEI = it->mSEInstance;
	//SEI (for now cushion) need to know all of this
	/*MWBase::GOAPStatus statusinput(GOAPStatus::AWARE_OF_OBJECT_WITH_STATUS, input.mExtraData, input.mAmount);
	node->mInputs.push_back(statusinput);
	MWBase::GOAPStatus statusoutput(output.mStatusType, output.mExtraData, output.mAmount);*/
	/*node->mOutputs.push_back(statusoutput);*/

	
}


SmartEntityCushionInstance::SmartEntityCushionInstance(std::string refid, ESM::RefNum refnum, int pings)
{
	std::cout << "new cushion" << std::endl;
	mPingCount = pings;
	mRefId = refid;
	mStatusList.push_back("comfortable");

	std::shared_ptr<MWBase::GOAPNodeData> node(new MWBase::GOAPNodeData);
	MWBase::GOAPStatus output(MWBase::GOAPStatus::VITALS, "stress", -1);
	MWBase::GOAPStatus input(MWBase::GOAPStatus::STATUS_VOID, "void", 0);
	node->mOutputs.push_back(output);
	node->mInputs.push_back(input);
	mGOAPNodeData.push_back(node);



	


}

std::string SmartEntityCushionInstance::getSaveString()
{
	return std::to_string(mPingCount);
}

bool SmartEntityCushionInstance::isAvailableForUse()
{
	return false;
}

MWBase::BehaviorObject * SmartEntityCushionInstance::useWorldInstance(MWBase::Life * user)
{
	mCurrentUserCount = 1;
	//new BOCushionSit(user);
	
	return nullptr;
}

BOCushionSit::BOCushionSit(int valence)
{






}

void BOCushionSit::getDebugInfo()
{
}

MWBase::BOReturn BOCushionSit::update(float time, MWWorld::Ptr ownerptr)
{
	return MWBase::BOReturn();
}

MWBase::BOReturn BOCushionSit::start()
{
	

	return MWBase::BOReturn();
}
