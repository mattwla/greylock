#include "subbrainusese.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwworld/class.hpp"
#include "../mwbase/tasksmanager.hpp"
#include <unordered_map>

//#include "../mwtasks/tasksmanagerimp.hpp"

#include "../mwmechanics/npcstats.hpp"


#include "../mwmechanics/aiactivate.hpp"


MWBase::SubBrainUseSE::SubBrainUseSE(MWBase::Life * life)
{
	mOwnerLife = life;
	mUseSEInWorldBO = new BOUseSEInWorld();	
	mGOAPNodes.push_back(mUseSEInWorldBO->getGOAPNode());
}

void MWBase::SubBrainUseSE::calculate(MWBase::Awareness * awareness)
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

std::string MWBase::SubBrainUseSE::getID()
{
	return std::string();
}

void MWBase::SubBrainUseSE::getDebugInfo()
{
}

std::vector <std::shared_ptr<MWBase::GOAPNodeData>> MWBase::SubBrainUseSE::getMatchingBehaviorObjects(MWBase::GOAPStatus status)
{
	
	std::vector<std::shared_ptr<GOAPNodeData>> result;


	//search through all SEIs in sensory stores
	//see if SEIs have a use function that offers a matching status
	//also store if SEIs need an input
	//If so, build a node with associated cost and required input
	
	//if (status.mStatusType == MWBase::GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY) //does npc want something in its inventory, we can help.
	{
		typedef std::unordered_map<ESM::RefNum, SensoryLink> linklist;
		
		//search memory for items that match status, create a node for each and cost dependant on distance to npc.
		
		MWBase::SensoryLinkStore * sensorystore = mOwnerLife->mAwareness->getSensoryLinkStore();
		linklist currentlinks = sensorystore->mSensoryLinks;
		//std::vector<SensoryLink> currentlinks = sensorystore->mCurrentSensoryLinks;



		for (linklist::iterator it = currentlinks.begin(); it != currentlinks.end(); it++)
		{
			typedef std::vector<std::shared_ptr<MWBase::GOAPNodeData>> GOAPNodeDatalist;
			GOAPNodeDatalist glo = it->second.mSEInstance->getGOAPNodeData();
				//->second->mSEInstance->
			for (GOAPNodeDatalist::iterator it2 = glo.begin(); it2 != glo.end(); it2++)
			{

				
				//need input and output
				//only does 1 of each for now, will be fixed later for multiple
				MWBase::GOAPStatus output = it2->get()->mOutputs[0];
				MWBase::GOAPStatus input = it2->get()->mInputs[0];
				bool match = output == status;
				if (match && it->second.mSEInstance->isAvailableForUse())
				{
					//MWX FIX ME 

					std::shared_ptr<GOAPNodeData> node(new GOAPNodeData);
					node->mBehaviorObject = mUseSEInWorldBO;
					node->mSEI = it->second.mSEInstance;
					//SEI (for now cushion) need to know all of this
					MWBase::GOAPStatus statusinput(input.mStatusType, input.mExtraData, input.mAmount);
					node->mInputs.push_back(statusinput);
					MWBase::GOAPStatus statusoutput(output.mStatusType, output.mExtraData, output.mAmount);
					node->mOutputs.push_back(statusoutput);


					node->mId = "Use SEI in world - location known.";
					std::cout << node->mId << std::endl;
					node->mCost = getCost(it->second.mSEInstance); //distance is fine for now.
					result.push_back(node);
				}
			}





			//if (it->mSEInstance->hasStatus(status.mExtraData))
			//{
			//	//a mess....
			//	std::shared_ptr<GOAPNodeData> node(new GOAPNodeData);
			//	node->mBehaviorObject = mUseSEInWorldBO;
			//	node->mSEI = it->mSEInstance;
			//	MWBase::GOAPStatus statusinput(GOAPStatus::AWARE_OF_OBJECT_WITH_STATUS, status.mExtraData, 1);
			//	node->mInputs.push_back(statusinput);
			//	MWBase::GOAPStatus statusoutput(GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY, status.mExtraData, 1);
			//	node->mOutputs.push_back(statusoutput);
			//	node->mId = "Get From World Node - known location";
			//	node->mCost = getCost(it->mSEInstance);
			//	result.push_back(node);
			//}
		}
		
		//if (result.size() == 0) //we didn't know of anything to get, make a node requesting a node that can tell us where an item is.
		//{
		//	std::shared_ptr<GOAPNodeData> node(new GOAPNodeData);
		//	node->mBehaviorObject = mUseSEInWorldBO;
		//	MWBase::GOAPStatus statusinput(GOAPStatus::AWARE_OF_OBJECT_WITH_STATUS, status.mExtraData, 1);
		//	node->mInputs.push_back(statusinput);
		//	MWBase::GOAPStatus statusoutput(GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY, status.mExtraData, 1);
		//	node->mOutputs.push_back(statusoutput);
		//	node->mId = "Get From World Node - location unknown";
		//	//std::cout << "Get From World Node - location unknown" << std::endl;
		//	result.push_back(node);
		//}	

	}
	return result;
}

int MWBase::SubBrainUseSE::getCost(MWBase::SmartEntityInstance * sei)
{
	MWWorld::Ptr seiptr = sei->getPtr();
	MWWorld::Ptr npcptr = mOwnerLife->mPtr;
	float distance = (npcptr.getRefData().getPosition().asVec3() - seiptr.getRefData().getPosition().asVec3()).length2();
	//std::cout << "distance to get object" + std::to_string(distance) << std::endl;
	return distance;
}

bool MWBase::SubBrainUseSE::InUsingRange(MWBase::SmartEntityInstance * sei)
{
	MWWorld::Ptr seiptr = sei->getPtr();
	MWWorld::Ptr npcptr = mOwnerLife->mPtr;
	float distance = (npcptr.getRefData().getPosition().asVec3() - seiptr.getRefData().getPosition().asVec3()).length2();
	//std::cout << "distance to get object" + std::to_string(distance) << std::endl;
	return distance < 1000.0f;
}

MWBase::BOReturn MWBase::BOUseSEInWorld::update(float time, MWWorld::Ptr ownerptr)
{
	if (mUsingSubBehavior)
	{
		MWBase::BOReturn status = mSubBehavior->update(time, mOwnerLife->mPtr);
		return status;
	}



	MWBase::JourneyManager * journeymanager = mOwnerLife->mJourneyManager;

	bool noticedSEUnavailable = false;
	noticedSEUnavailable = checkSEUnavailableNotice();
	if (noticedSEUnavailable)
	{
		std::cout << "SE now in use." << std::endl;
		journeymanager->cancelJourney(10);
		return FAILED;
	

		//Look around for item
		//if can't find, give up
	}

	if (inUsingDistance())
	{
		std::cout << "in using distance" << std::endl;
		journeymanager->cancelJourney(10);
		useSE(); //wut if pickup didn't work? How can we guarantee? Maybe verify status here?
		//return COMPLETE;
		//grab
	}
	else if (mInJourney)
	{
		MWBase::JourneyStatus jstatus = JOURNEY_IN_PROGRESS;
		jstatus = journeymanager->updateJourney(10);
		if (jstatus == JOURNEY_COMPLETED)
		{
			mInJourney = false;
		}
	
	}
	else
	{
		//std::cout << "new journey" << std::endl;
		mInJourney = journeymanager->requestNewJourney(mExpectedPosition);
		if (!mInJourney)
		{
			//return that we can't do this BO
		}
		
	}

//	mInJourney = mOwnerLife->mJourneyManager->requestNewJourney();
	
	return MWBase::IN_PROGRESS;
}

bool MWBase::BOUseSEInWorld::checkSEUnavailableNotice()
{
	bool canseepos = MWBase::Environment::get().getAwarenessReactionsManager()->sightToPosCheck(mOwnerLife->mPtr, mExpectedPosition);
	if (canseepos)
	{
		if (!mSEITarget->isAvailableForUse())
			return true;
	}

	return false;

}

void MWBase::BOUseSEInWorld::useSE()
{
	std::cout << "attempted to use SE in world" << std::endl;
	MWWorld::Ptr itemPtr = mSEITarget->getPtr();
	MWWorld::Ptr ownerPtr = mOwnerLife->mPtr;
	
	mSubBehavior = mSEITarget->useWorldInstance(mOwnerLife);
	if (mSubBehavior)
	{
		
		mUsingSubBehavior = true;
	}
	else
	{
		std::cout << "no sub behavior given" << std::endl;
	}


}

bool MWBase::BOUseSEInWorld::inUsingDistance()
{
	MWWorld::Ptr seiptr = mSEITarget->getPtr();
	MWWorld::Ptr npcptr = mOwnerLife->mPtr;
	float distance = (npcptr.getRefData().getPosition().asVec3() - seiptr.getRefData().getPosition().asVec3()).length2();
	//std::cout << "distance to get object" + std::to_string(distance) << std::endl;
	return distance < mSEITarget->getActivationDistance();

}

MWBase::BOReturn MWBase::BOUseSEInWorld::start()
{
	if (!mSEITarget)
	{
		std::cout << "get has no target, can't do!" << std::endl;
		return BOReturn::FAILED;
	}
	std::cout << "started use se in world" << std::endl;
	ESM::Position pos = mSEITarget->getPtr().getRefData().getPosition();
	mExpectedPosition = pos;
	return BOReturn::IN_PROGRESS;
}

void MWBase::BOUseSEInWorld::getDebugInfo()
{
}

MWBase::BOUseSEInWorld::BOUseSEInWorld()
{
	mInJourney = false;
	std::cout << "made UseSEInWorld BO" << std::endl;
	MWBase::GOAPStatus statusinput(GOAPStatus::AWARE_OF_OBJECT_WITH_STATUS, "", 1);
	std::shared_ptr<GOAPNodeData> gd(new GOAPNodeData());
	mGOAPNodeData = gd;
	mGOAPNodeData->mInputs.push_back(statusinput);
	MWBase::GOAPStatus statusoutput(GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY, "", 1);
	mGOAPNodeData->mOutputs.push_back(statusoutput);
	mGOAPNodeData->mBehaviorObject = this;
	mGOAPNodeData->mId = "BO GET FROM WORLD";
}