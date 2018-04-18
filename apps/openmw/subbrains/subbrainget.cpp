#include "subbrainget.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"

MWBase::SubBrainGet::SubBrainGet(MWBase::Life * life)
{
	mOwnerLife = life;
	mGetFromWorldBO = new BOGetFromWorld();	
	mGOAPNodes.push_back(mGetFromWorldBO->getGOAPNode());
}

void MWBase::SubBrainGet::calculate(MWBase::Awareness * awareness)
{
	std::vector<SensoryLink> linkstore = awareness->getSensoryLinkStore()->mCurrentSensoryLinks;
	std::vector<SensoryLink>::iterator it = linkstore.begin();
	while (it != linkstore.end())
	{
		//std::cout << "Oh I see" + it->mPtr.getCellRef().getRefId() << std::endl;
		//for every item, create a getBO
		it++;
	}
	//search through SEs that npcs can pick up, maybe even calculate cost to get each?
	//only search throug new SEs?
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
	
	if (status.mStatusType == MWBase::GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY)
	{
		typedef std::vector<SensoryLink> linklist;
		
		//search memory for items that match status, create a node for each and cost dependant on distance to npc.
		
		MWBase::SensoryLinkStore * sensorystore = mOwnerLife->mAwareness->getSensoryLinkStore();
		std::vector<SensoryLink> currentlinks = sensorystore->mCurrentSensoryLinks;

		for (linklist::iterator it = currentlinks.begin(); it != currentlinks.end(); it++)
		{
			if (it->mSEInstance->hasStatus(status.mExtraData))
			{
				std::shared_ptr<GOAPData> node(new GOAPData);
				node->mBehaviorObject = mGetFromWorldBO;
				node->mSEI = it->mSEInstance;
				MWBase::GOAPStatus statusinput(GOAPStatus::AWARE_OF_OBJECT_WITH_STATUS, status.mExtraData, 1);
				node->mInputs.push_back(statusinput);
				MWBase::GOAPStatus statusoutput(GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY, status.mExtraData, 1);
				node->mOutputs.push_back(statusoutput);
				node->mId = "Get From World Node - known location";
				node->mCost = 10;
				result.push_back(node);
			}
		}
		
		if (result.size() == 0) //we didn't know of anything to get, make a node in case something can give us the info we need.
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

MWBase::BOReturn MWBase::BOGetFromWorld::update(float time)
{
	return MWBase::IN_PROGRESS;
}

void MWBase::BOGetFromWorld::getDebugInfo()
{
}

MWBase::BOGetFromWorld::BOGetFromWorld()
{
	mIsDesire = false;
	std::cout << "made GetFromWorld BO" << std::endl;
	mValence = 1;
	MWBase::GOAPStatus statusinput(GOAPStatus::AWARE_OF_OBJECT_WITH_STATUS, "", 1);
	std::shared_ptr<GOAPData> gd(new GOAPData());
	
	mGOAPData = gd;
	mGOAPData->mInputs.push_back(statusinput);
	MWBase::GOAPStatus statusoutput(GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY, "", 1);
	mGOAPData->mOutputs.push_back(statusoutput);
	mGOAPData->mBehaviorObject = this;
	mGOAPData->mId = "BO GET FROM WORLD";
}