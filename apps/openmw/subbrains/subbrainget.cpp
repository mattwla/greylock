#include "subbrainget.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"

#include "../mwtasks/tasksmanagerimp.hpp"

MWBase::SubBrainGet::SubBrainGet(MWBase::Life * life)
{
	mOwnerLife = life;
	mGetFromWorldBO = new BOGetFromWorld();	
	mGOAPNodes.push_back(mGetFromWorldBO->getGOAPNode());
}

void MWBase::SubBrainGet::calculate(MWBase::Awareness * awareness)
{
	//Does nothing now.
	//What if it cached nodes?
	/*std::vector<SensoryLink> linkstore = awareness->getSensoryLinkStore()->mCurrentSensoryLinks;
	std::vector<SensoryLink>::iterator it = linkstore.begin();
	while (it != linkstore.end())
	{
		it++;
	}*/
}

std::string MWBase::SubBrainGet::getID()
{
	return std::string();
}

void MWBase::SubBrainGet::getDebugInfo()
{
}

std::vector <std::shared_ptr<MWBase::GOAPData>> MWBase::SubBrainGet::getMatchingBehaviorObjects(MWBase::GOAPStatus status)
{
	
	std::vector<std::shared_ptr<GOAPData>> result;
	
	if (status.mStatusType == MWBase::GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY) //does npc want something in its inventory, we can help.
	{
		typedef std::vector<SensoryLink> linklist;
		
		//search memory for items that match status, create a node for each and cost dependant on distance to npc.
		
		MWBase::SensoryLinkStore * sensorystore = mOwnerLife->mAwareness->getSensoryLinkStore();
		std::vector<SensoryLink> currentlinks = sensorystore->mCurrentSensoryLinks;

		for (linklist::iterator it = currentlinks.begin(); it != currentlinks.end(); it++)
		{
			if (it->mSEInstance->hasStatus(status.mExtraData))
			{
				//a mess....
				std::shared_ptr<GOAPData> node(new GOAPData);
				node->mBehaviorObject = mGetFromWorldBO;
				node->mSEI = it->mSEInstance;
				MWBase::GOAPStatus statusinput(GOAPStatus::AWARE_OF_OBJECT_WITH_STATUS, status.mExtraData, 1);
				node->mInputs.push_back(statusinput);
				MWBase::GOAPStatus statusoutput(GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY, status.mExtraData, 1);
				node->mOutputs.push_back(statusoutput);
				node->mId = "Get From World Node - known location";
				node->mCost = getCost(it->mSEInstance);
				result.push_back(node);
			}
		}
		
		if (result.size() == 0) //we didn't know of anything to get, make a node requesting a node that can tell us where an item is.
		{
			std::shared_ptr<GOAPData> node(new GOAPData);
			node->mBehaviorObject = mGetFromWorldBO;
			MWBase::GOAPStatus statusinput(GOAPStatus::AWARE_OF_OBJECT_WITH_STATUS, status.mExtraData, 1);
			node->mInputs.push_back(statusinput);
			MWBase::GOAPStatus statusoutput(GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY, status.mExtraData, 1);
			node->mOutputs.push_back(statusoutput);
			node->mId = "Get From World Node - location unknown";
			std::cout << "Get From World Node - location unknown" << std::endl;
			result.push_back(node);
		}	

	}
	return result;
}

int MWBase::SubBrainGet::getCost(MWBase::SmartEntityInstance * sei)
{
	MWWorld::Ptr seiptr = sei->getPtr();
	MWWorld::Ptr npcptr = mOwnerLife->mPtr;
	float distance = (npcptr.getRefData().getPosition().asVec3() - seiptr.getRefData().getPosition().asVec3()).length2();
	std::cout << "distance to get object" + std::to_string(distance) << std::endl;
	return distance;
}

bool MWBase::SubBrainGet::InGrabbingRange(MWBase::SmartEntityInstance * sei)
{
	MWWorld::Ptr seiptr = sei->getPtr();
	MWWorld::Ptr npcptr = mOwnerLife->mPtr;
	float distance = (npcptr.getRefData().getPosition().asVec3() - seiptr.getRefData().getPosition().asVec3()).length2();
	std::cout << "distance to get object" + std::to_string(distance) << std::endl;
	return distance < 1000.0f;
}

MWBase::BOReturn MWBase::BOGetFromWorld::update(float time, MWWorld::Ptr ownerptr)
{
	MWBase::JourneyManager * journeymanager = mOwnerLife->mJourneyManager;

	if (inGrabbingDistance())
	{
		std::cout << "im grabbing distance" << std::endl;
		//grab
	}
	else if (mInJourney)
	{
		MWBase::JourneyStatus jstatus = JOURNEY_IN_PROGRESS;
		jstatus = journeymanager->updateJourney(10);
		if (jstatus = JOURNEY_COMPLETED)
		{
			mInJourney = false;
		}
	
	}
	else
	{
		ESM::Position pos = mSEITarget->getPtr().getRefData().getPosition();
		mInJourney = journeymanager->requestNewJourney(pos);
		if (!mInJourney)
		{
			//return that we can't do this BO
		}
		
	}




//	mInJourney = mOwnerLife->mJourneyManager->requestNewJourney();
	
	
	return MWBase::IN_PROGRESS;


}

bool MWBase::BOGetFromWorld::inGrabbingDistance()
{
	MWWorld::Ptr seiptr = mSEITarget->getPtr();
	MWWorld::Ptr npcptr = mOwnerLife->mPtr;
	float distance = (npcptr.getRefData().getPosition().asVec3() - seiptr.getRefData().getPosition().asVec3()).length2();
	std::cout << "distance to get object" + std::to_string(distance) << std::endl;
	return distance < 1000.0f;

}

void MWBase::BOGetFromWorld::getDebugInfo()
{
}

MWBase::BOGetFromWorld::BOGetFromWorld()
{
	mInJourney = false;
	std::cout << "made GetFromWorld BO" << std::endl;
	MWBase::GOAPStatus statusinput(GOAPStatus::AWARE_OF_OBJECT_WITH_STATUS, "", 1);
	std::shared_ptr<GOAPData> gd(new GOAPData());
	mGOAPData = gd;
	mGOAPData->mInputs.push_back(statusinput);
	MWBase::GOAPStatus statusoutput(GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY, "", 1);
	mGOAPData->mOutputs.push_back(statusoutput);
	mGOAPData->mBehaviorObject = this;
	mGOAPData->mId = "BO GET FROM WORLD";
}