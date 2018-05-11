#include "subbrainget.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwworld/class.hpp"
#include "../mwbase/tasksmanager.hpp"

//#include "../mwtasks/tasksmanagerimp.hpp"

#include "../mwmechanics/npcstats.hpp"


#include "../mwmechanics/aiactivate.hpp"


MWBase::SubBrainGet::SubBrainGet(MWBase::Life * life)
{
	mOwnerLife = life;
	mGetFromWorldBO = new BOGetFromWorld();	
	//mGOAPNodes.push_back(mGetFromWorldBO->getGOAPNode());
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

std::vector <std::shared_ptr<MWBase::GOAPNodeData>> MWBase::SubBrainGet::getMatchingBehaviorObjects(MWBase::GOAPStatus status)
{
	
	std::vector<std::shared_ptr<GOAPNodeData>> result;
	
	if (status.mStatusType == MWBase::GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY) //does npc want something in its inventory, we can help.
	{
		typedef std::unordered_map<ESM::RefNum, SensoryLink> linklist;
		
		//search memory for items that match status, create a node for each and cost dependant on distance to npc.
		
		MWBase::SensoryLinkStore * sensorystore = mOwnerLife->mAwareness->getSensoryLinkStore();
		linklist currentlinks = sensorystore->mSensoryLinks;

		for (linklist::iterator it = currentlinks.begin(); it != currentlinks.end(); it++)
		{
			if (it->second.mSEInstance->hasStatus(status.mExtraData))
			{
				MWBase::GOAPStatus statusinput(GOAPStatus::AWARE_OF_OBJECT_WITH_STATUS, status.mExtraData, 1);
				MWBase::GOAPStatus statusoutput(GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY, status.mExtraData, 1);
				std::shared_ptr<GOAPNodeData> node(new GOAPNodeData(statusinput, statusoutput, mGetFromWorldBO, it->first, 1, "get from world" + it->second.mSEInstance->getRefId() ));
				result.push_back(node);
			}
		}
		
		if (result.size() == 0) //we didn't know of anything to get, make a node requesting a node that can tell us where an item is.
		{
			//std::shared_ptr<GOAPNodeData> node(new GOAPNodeData);
			//node->mBehaviorObject = mGetFromWorldBO;
			//MWBase::GOAPStatus statusinput(GOAPStatus::AWARE_OF_OBJECT_WITH_STATUS, status.mExtraData, 1);
			//node->mInputs.push_back(statusinput);
			//MWBase::GOAPStatus statusoutput(GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY, status.mExtraData, 1);
			//node->mOutputs.push_back(statusoutput);
			//node->mId = "Get From World Node - location unknown";
			////std::cout << "Get From World Node - location unknown" << std::endl;
			//result.push_back(node);
		}	

	}
	return result;
}

int MWBase::SubBrainGet::getCost(MWBase::SmartEntityInstance * sei)
{
	MWWorld::Ptr seiptr = sei->getPtr();
	MWWorld::Ptr npcptr = mOwnerLife->mPtr;
	float distance = (npcptr.getRefData().getPosition().asVec3() - seiptr.getRefData().getPosition().asVec3()).length2();
	//std::cout << "distance to get object" + std::to_string(distance) << std::endl;
	return distance;
}

bool MWBase::SubBrainGet::InGrabbingRange(MWBase::SmartEntityInstance * sei)
{
	MWWorld::Ptr seiptr = sei->getPtr();
	MWWorld::Ptr npcptr = mOwnerLife->mPtr;
	float distance = (npcptr.getRefData().getPosition().asVec3() - seiptr.getRefData().getPosition().asVec3()).length2();
	//std::cout << "distance to get object" + std::to_string(distance) << std::endl;
	return distance < 1000.0f;
}

MWBase::BOReturn MWBase::BOGetFromWorld::update(float time, MWWorld::Ptr ownerptr)
{



	MWBase::JourneyManager * journeymanager = mOwnerLife->mJourneyManager;


	if (mStopRequested)
	{
		journeymanager->cancelJourney(10);
		return COMPLETE;
		//mwx fix me hack
	}


	bool noticedItemGone = false;
	noticedItemGone = checkItemGoneNotice();
	if (noticedItemGone)
	{
		std::cout << "item changed position" << std::endl;
		mOwnerLife->mAwareness->getSensoryLinkStore()->removeSensoryLink(mSEITarget->getPtr().getCellRef().getRefNum());
		journeymanager->cancelJourney(10);
		return FAILED;
	

		//Look around for item
		//if can't find, give up
	}

	if (inGrabbingDistance())
	{
		std::cout << "im grabbing distance" << std::endl;
		journeymanager->cancelJourney(10);
		pickupItem(); //wut if pickup didn't work? How can we guarantee? Maybe verify status here?
		
		return COMPLETE;
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

bool MWBase::BOGetFromWorld::checkItemGoneNotice()
{
	osg::Vec3f ininv(0, 0, 0);

	//HACK HACK MWX FIX ME
	//if npc magically sense that item location is 0 0 0 which often means in someones inventory than report failure, so hacky.
	if (mSEITarget->getPtr().getRefData().getPosition().asVec3() == ininv)
	{
		return true;
	}

	bool canseepos = MWBase::Environment::get().getAwarenessReactionsManager()->sightToPosCheck(mOwnerLife->mPtr, mExpectedPosition);
	if (canseepos)
	{
		bool changedpos = mSEITarget->getPtr().getRefData().getPosition().asVec3() != mExpectedPosition.asVec3();
		osg::Vec3f pos1(mSEITarget->getPtr().getRefData().getPosition().asVec3());
		// std::cout << pos1[0] << std::endl;
		osg::Vec3f pos2(mExpectedPosition.asVec3());
		//std::cout << pos2[0] << std::endl;
		if (changedpos)
			return true;	
	}

	
	return false;
	

}

void MWBase::BOGetFromWorld::pickupItem()
{
	MWWorld::Ptr itemPtr = mSEITarget->getPtr();
	MWWorld::Ptr ownerPtr = mOwnerLife->mPtr;
	MWMechanics::AiSequence& seq = ownerPtr.getClass().getCreatureStats(ownerPtr).getAiSequence();
	//bool currentlyActive = MWBase::Environment::get().getLifeManager()->inActiveRange(mNpcPtr);
	/*if (currentlyActive)*/
	{
		//seq.stack(MWMechanics::AiActivate(itemPtr), ownerPtr);
		MWBase::Environment::get().getWorld()->activate(itemPtr, ownerPtr);
		std::cout << "activated" << std::endl;
	}
	/*else
	{
		MWBase::Environment::get().getWorld()->activate(itemPtr, mNpcPtr);
	}*/
}

bool MWBase::BOGetFromWorld::inGrabbingDistance()
{
	MWWorld::Ptr seiptr = mSEITarget->getPtr();
	MWWorld::Ptr npcptr = mOwnerLife->mPtr;
	float distance = (npcptr.getRefData().getPosition().asVec3() - seiptr.getRefData().getPosition().asVec3()).length2();
	//std::cout << "distance to get object" + std::to_string(distance) << std::endl;
	return distance < 2000.0f;

}

MWBase::BOReturn MWBase::BOGetFromWorld::start()
{
	//if (!mSEITarget)
	//{
	//	std::cout << "get has no target, can't do!" << std::endl;
	//	return BOReturn::FAILED;
	//}
	std::cout << "started getfromworld" << std::endl;
	mSEITarget = mOwnerLife->mAwareness->getSensoryLinkStore()->mSensoryLinks[mTargetRefNum].mSEInstance;
	//ESM::Position pos = mSEITarget->getPtr().getRefData().getPosition();
	mExpectedPosition = mOwnerLife->mAwareness->getSensoryLinkStore()->mSensoryLinks[mTargetRefNum].mLastPosition;
	return BOReturn::IN_PROGRESS;
}

void MWBase::BOGetFromWorld::getDebugInfo()
{
}

MWBase::BOGetFromWorld::BOGetFromWorld()
{
	mInJourney = false;
	std::cout << "made GetFromWorld BO" << std::endl;
	/*MWBase::GOAPStatus statusinput(GOAPStatus::AWARE_OF_OBJECT_WITH_STATUS, "", 1);
	std::shared_ptr<GOAPNodeData> gd(new GOAPNodeData());
	mGOAPNodeData = gd;
	mGOAPNodeData->mInputs.push_back(statusinput);
	MWBase::GOAPStatus statusoutput(GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY, "", 1);
	mGOAPNodeData->mOutputs.push_back(statusoutput);
	mGOAPNodeData->mBehaviorObject = this;
	mGOAPNodeData->mId = "BO GET FROM WORLD";*/
}