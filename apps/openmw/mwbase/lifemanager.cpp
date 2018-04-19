#include "lifemanager.hpp"
#include <iostream>
#include "../mwworld/cellstore.hpp"
#include "../mwworld/cells.hpp" 
#include "../subbrains/subbrain.hpp"
#include "../subbrains/subbrainhunger.hpp"
#include "../subbrains/subbrainget.hpp"
#include "../subbrains/subbraininventory.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../gllifemanager/goap.hpp"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include "../mwworld/class.hpp"


namespace MWBase
{
	MWBase::SmartEntityInstance * Life::getSEIWithStatusFromInventory(std::string status)
	{
		MWWorld::Ptr ptr = mPtr;

		auto sem = MWBase::Environment::get().getSmartEntitiesManager();
		MWWorld::InventoryStore &inventoryStore = ptr.getClass().getInventoryStore(ptr);
		for (MWWorld::ContainerStoreIterator it = inventoryStore.begin(); it != inventoryStore.end(); ++it)
		{
			if (sem->hasSmartInstance(*it))
			{
				std::cout << "found matching SI" << std::endl;
				std::cout << (it)->getCellRef().getRefId() << std::endl;
				auto sei = sem->getSmartEntityInstance(*it);
				if (sei->hasStatus(status))
					return sei;
			}
		}
	

		return nullptr;
	}

	//LIFE
	void Life::getDebugInfo()
	{
		std::cout << mId << std::endl;
		std::cout << "Hunger: " + std::to_string(mVitals.mHunger) << std::endl;
		std::cout << "Sleepiness: " + std::to_string(mVitals.mSleepiness) << std::endl;
		mSubBrainsManager->logDesires();
		mSubBrainsManager->logWorldstate();
		if (mHasIntention)
		{
			std::cout << "intention info" << std::endl;
			unsigned int itx = 0;
			while (itx < mCurrentIntentionPlan.mGOAPDataList.size())
			{
				std::cout << mCurrentIntentionPlan.mGOAPDataList[itx]->mId << std::endl;
				itx++;
			}
		}
		else
			std::cout << "has no intention" << std::endl;
	}

	void Life::update(float duration)
	{
		metabolize(duration);
		mAwareness->refresh();
		mSubBrainsManager->calculate(mAwareness);
		std::vector<GOAPDesire> GOAPDesires = mSubBrainsManager->getGOAPDesires();
		//What if we already have an intention? we need to weight it against new desires. Right now only one intention at a time.
		if (GOAPDesires.size() > 0 && !mHasIntention)
		{
			prioritizeDesires(GOAPDesires);
			bool foundPossibleIntention = false;
			unsigned int itx = 0;
			while (!foundPossibleIntention && itx < GOAPDesires.size())
			{
				IntentionPlan newplan = mSubBrainsManager->createIntention(GOAPDesires[itx].mStatus, mPtr);
				if (newplan.mPlanComplete)
				{
					mCurrentIntentionPlan = newplan;
					mHasIntention = true;
					mCurrentIntentionPlan.mCurrentStep = mCurrentIntentionPlan.mGOAPDataList.size() - 1; // set at last step, we go backwards
					std::cout << "I have an intention plan" << std::endl;
					foundPossibleIntention = true;
				}
				else
				{
					std::cout << "skipping desire which we can not meet" << std::endl;
				}
				itx += 1;
			}
		}
	
		if (mHasIntention && mCurrentIntentionPlan.mCurrentBehaviorObject == 0) 	//Need to start an intention plan.
		{
			int step = mCurrentIntentionPlan.mCurrentStep;
			MWBase::GOAPData * currentnode = mCurrentIntentionPlan.mGOAPDataList[step].get();
			std::cout << "trying to:..." + currentnode->mId << std::endl;
			BehaviorObject * newbo = currentnode->mBehaviorObject->Clone();
			newbo->setTarget(currentnode->mSEI);
			mCurrentIntentionPlan.mCurrentBehaviorObject = newbo;
			mCurrentIntentionPlan.mCurrentBehaviorObject->setOwner(this);
			mCurrentIntentionPlan.mCurrentBehaviorObject->start();


		}
		else if (mHasIntention) //need to continue an intention plan
		{
			BehaviorObject * bo = mCurrentIntentionPlan.mCurrentBehaviorObject;
			//bo->setOwner(this);

			MWBase::BOReturn status = bo->update(duration, mPtr);
			if (status == BOReturn::IN_PROGRESS)
			{
				//std::cout << "BO In progress" << std::endl;
			}
			else if (status == BOReturn::COMPLETE)
			{
				delete bo;
				mCurrentIntentionPlan.mCurrentBehaviorObject = 0;
				mCurrentIntentionPlan.mCurrentStep -= 1;
				if (mCurrentIntentionPlan.mCurrentStep = -1)
				{
					mHasIntention = false;

				}

			}
		}
	}

	void Life::metabolize(float duration)
	{
		mVitals.mHunger += duration / 300.0f;
		mVitals.mSleepiness += duration / 2000.f;
	}

	void Life::prioritizeDesires(std::vector<GOAPDesire>& desires)
	{
		//weigh against current one too
		desires;
	}

	//LIFE MANAGER

	void LifeManager::loadGame(boost::filesystem::path path)
	{

		mLifeList.clear();
		mLifeList.shrink_to_fit();
		//get runtime id count
		std::ifstream in(path.string() + "LMS");

		if (!in.is_open())
			std::cout << "NOT OPEN" << std::endl;
		else
			std::cout << "OPEN" << std::endl;

		std::string line;

		typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;


		while (getline(in, line))
		{
			if (line == "v1")
			{
				continue;
			}

			Tokenizer tok(line);
			//		int idx = 0;
			std::vector<std::string> cache;
			for (Tokenizer::iterator it(tok.begin()), end(tok.end()); it != end; ++it) //iterate through the line, values seperated by commas
			{

				cache.push_back(*it);
			}
			MWBase::Life * life = new Life(cache[0]);
			life->mVitals.loadState(cache[1]);
			mLifeList.push_back(life);
		}
	}

	void LifeManager::saveGame(boost::filesystem::path path)
	{
		std::string path_s = path.string();
		path_s += "LMS";
		path = path_s;
		std::vector<Life*>::iterator it = mLifeList.begin();
		//SmartInstanceMap::iterator it = mSmartInstanceMap.begin();
		std::vector<std::string>serializedinstances;
		ESM::RefNum refnum;
		while (it != mLifeList.end())
		{
			std::string savestring;
			MWBase::Life* life = *it;
			std::string name = life->mId;
			std::string vitalssave = life->mVitals.getSaveState();
			savestring = name + "," + vitalssave;

			//get subbrain info from npc
			//std::vector<std::string> subbrainsave = life->mSubBrainsManager->getSaveStates();
			serializedinstances.push_back(savestring);
			it++;
		}
		boost::filesystem::ofstream filestream(path, std::ios::binary);
		filestream << "v1" << std::endl;
		std::vector<std::string>::iterator itS = serializedinstances.begin();
		while (itS != serializedinstances.end())
		{
			filestream << *itS << std::endl;
			itS++;
		}
	}

	//VITALS
	std::string Vitals::getSaveState()
	{
		std::string save;
		save += std::to_string(mHunger) + "-";
		save += std::to_string(mSleepiness);
		return save;
	}

	void Vitals::loadState(std::string data)
	{
		std::vector<int> cache;
		typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
		boost::char_separator<char> sep("-");
		Tokenizer tok(data, sep);
		for (Tokenizer::iterator tok_iter = tok.begin();
			tok_iter != tok.end(); ++tok_iter)
		{
			cache.push_back(std::stoi(*tok_iter));
		}
		mHunger = cache[0];
		mSleepiness = cache[1];
	}
}


//SUB BRAINS

void MWBase::SubBrainsManager::seperateCompletePlans(std::vector<IntentionPlan>& planlist, std::vector<IntentionPlan>& completelist, MWWorld::Ptr ptr)
{
	typedef std::vector<IntentionPlan> plans;
	plans newplanlist;

	for (plans::iterator it = planlist.begin(); it != planlist.end(); it++)
	{
		std::shared_ptr<GOAPData> currentplan = it->mGOAPDataList.back();
		bool plancomplete = evaluateGOAPStatus(currentplan->mInputs[0], ptr); //currently only checks 1 status;
		if (plancomplete)
		{
			it->mPlanComplete = true;
			completelist.push_back(*it);
		}
		else
		{
			newplanlist.push_back(*it);
		}
	}

	planlist = newplanlist;

}

bool MWBase::SubBrainsManager::evaluateGOAPStatus(MWBase::GOAPStatus status, MWWorld::Ptr ptr)
{
	switch (status.mStatusType) {
		case MWBase::GOAPStatus::HAS_ID_IN_INVENTORY:
			//evaluate inventory for specific id
			break;
		case MWBase::GOAPStatus::VITALS:
			//evaluate vitals
			break;
		case MWBase::GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY:
			//evalualte if an object with a certain status is in inventory
			return hasObjectStatusInInventory(status, ptr);
			break;
		case MWBase::GOAPStatus::AWARE_OF_OBJECT_WITH_STATUS:
			//evaluate if npc has object in its awareness
			//return
			return hadObjectStatusInAwareness(status.mExtraData, ptr);
			break;
	}
	
	std::cout << "status" + status.mExtraData + "not true" << std::endl;
	return false;
}


MWBase::IntentionPlan MWBase::SubBrainsManager::createIntention(MWBase::GOAPStatus status, MWWorld::Ptr ptr)
{
	typedef std::vector<IntentionPlan> planlist;
	planlist possibleplans;
	planlist completeplans;
	IntentionPlan emptyplan;
	emptyplan.mPlanComplete = false;
	typedef std::vector<std::shared_ptr<GOAPData>> nodechain;
	std::vector<nodechain> nodechainlist;
	
	//Seed the list, ask all subbrains if they have a behavior object that can meet our need.
	nodechain nc = querySubBrainsForGOAPMatches(status);
	for (nodechain::iterator itnc = nc.begin(); itnc != nc.end(); itnc++)
	{
		IntentionPlan plan;
		plan.mDesiredState = status;
		plan.mGOAPDataList.push_back(*itnc);
		possibleplans.push_back(plan);
	}

	if (possibleplans.size() == 0)
	{
		//No way to meet desire
		return emptyplan;
	}

	seperateCompletePlans(possibleplans, completeplans, ptr);

	//Now we have a list of BOs that can meet our needs... but those BOs in turn might have needs that are not met.
	//Iterate through the list, and whenever a BO has more than one way of being met, copy the nodechain and push it back, with a new branch for each possible method

	while (possibleplans.size() > 0)
	{		
		nodechain possiblepaths = querySubBrainsForGOAPMatches(possibleplans[0].mGOAPDataList.back()->mInputs[0]); //only checks first input, no support for multiple for inputs for now
		if (possiblepaths.size() == 0)
		{
			//std::cout << "dead end plan" << std::endl;
			//std::cout << possibleplans[0].mDesiredState.mExtraData << std::endl;
		}
		for (nodechain::iterator itb = possiblepaths.begin(); itb != possiblepaths.end(); itb++)
		{
			//copy plan
			IntentionPlan copiedplan = possibleplans[0];
			copiedplan.mGOAPDataList.push_back(*itb);
			possibleplans.push_back(copiedplan);
		}

		//erase old plan, also serves to trim deadend plans
		possibleplans.erase(possibleplans.begin());

		//remove complete plans
		seperateCompletePlans(possibleplans, completeplans, ptr);
	}

	//see if any are complete, if so move to a special complete vector
	//For rest, run them through matching thing again.
	//Should I return lowest cost one? But what if we want an alternate plan? Maybe optional blacklist parameter for when a plan fails (pass the status we couldn't make work)
	if (completeplans.size() > 0)
		return completeplans[0];
	else
		return emptyplan;
}

std::vector<std::shared_ptr<MWBase::GOAPData>> MWBase::SubBrainsManager::querySubBrainsForGOAPMatches(MWBase::GOAPStatus status)
{
	typedef std::vector<std::shared_ptr<MWBase::GOAPData>> goapdatalist;
	goapdatalist result;

	for (std::vector<MWBase::SubBrain*>::iterator it = mSubBrains.begin(); it != mSubBrains.end(); ++it)
	{
		//ask the subbrain what behavior objects it has that can meet this status
		goapdatalist nc = (*it)->getMatchingBehaviorObjects(status);
		//push them all into our result
		for (goapdatalist::iterator itg = nc.begin(); itg != nc.end(); itg++)
		{
			result.push_back(*itg);
		}	
		//onto next subbrain
	}
	return result;
}

bool MWBase::SubBrainsManager::hasObjectStatusInInventory(MWBase::GOAPStatus status, MWWorld::Ptr ptr)
{
	auto sem = MWBase::Environment::get().getSmartEntitiesManager();
	MWWorld::InventoryStore &inventoryStore = ptr.getClass().getInventoryStore(ptr);
	for (MWWorld::ContainerStoreIterator it = inventoryStore.begin(); it != inventoryStore.end(); ++it)
	{
		if (sem->hasSmartInstance(*it))
		{
			std::cout << "found matching SI" << std::endl;
			std::cout << (it)->getCellRef().getRefId() << std::endl;
			auto sei = sem->getSmartEntityInstance(*it);
			if (sei->hasStatus(status.mExtraData))
				return true;
		}
	}
	return false;
}

bool MWBase::SubBrainsManager::hadObjectStatusInAwareness(std::string status, MWWorld::Ptr ptr)
{
	typedef std::vector<SensoryLink> linklist;
	MWBase::SensoryLinkStore * sensorystore = mLife->mAwareness->getSensoryLinkStore();
	std::vector<SensoryLink> currentlinks = sensorystore->mCurrentSensoryLinks;
	for (linklist::iterator it = currentlinks.begin(); it != currentlinks.end(); it++)
	{
		if (it->mSEInstance->hasStatus(status))
		{
			return true;
		}
	}
	return false;
}

void MWBase::SubBrainsManager::calculate(MWBase::Awareness * awareness)
{
	mGOAPNodes.clear();
	mGOAPDesires.clear();
	for (std::vector<MWBase::SubBrain*>::iterator it = mSubBrains.begin(); it != mSubBrains.end(); ++it)
	{
		(*it)->calculate(awareness);
		
	
		std::vector<MWBase::GOAPDesire> gdesirelist = (*it)->getGOAPDesires();
		for (std::vector<MWBase::GOAPDesire>::iterator gdl = gdesirelist.begin(); gdl != gdesirelist.end(); gdl++)
		{
			mGOAPDesires.push_back(*gdl);
		}

	}

}

MWBase::SubBrainsManager::SubBrainsManager(MWBase::Life * life)
{
	mLife = life;
	//initialize default subbrains, this will have to be done on a per npc basis one day.
	SubBrain * sb = new SubBrainHunger(life);
	mSubBrains.push_back(sb);
	sb = new SubBrainGet(life);
	mSubBrains.push_back(sb);
	sb = new SubBrainInventory(life);
	mSubBrains.push_back(sb);
}

std::vector<std::string> MWBase::SubBrainsManager::getSaveStates()
{
	std::vector<std::string> temp;
	return temp;
}

void MWBase::SubBrainsManager::logDesires()
{

	//if (mDesires.size() == 0)
	//{
	//	std::cout << "no desires" << std::endl;
	//	return;
	//}
	//std::cout << "desires:" << std::endl;
	//typedef std::vector<MWBase::BehaviorObject*> desirelist;
	//for (desirelist::iterator it = mDesires.begin(); it != mDesires.end(); it++)
	//{
	//	(*it)->getDebugInfo();
	//}

}

void MWBase::SubBrainsManager::logWorldstate()
{

	/*if (mWorldState.size() == 0)
	{
		std::cout << "no world state" << std::endl;
		return;
	}
	std::cout << "WorldState:" << std::endl;
	typedef std::vector<WorldstateAtom> wslist;
	for (wslist::iterator it = mWorldState.begin(); it != mWorldState.end(); it++)
	{
		std::string value;
		if (it->mValue)
			value = "True";
		else
			value = "False";
		std::cout << it->mTag << std::endl;
		std::cout << value << std::endl;
	}*/

}

std::vector<MWBase::GOAPDesire> MWBase::SubBrainsManager::getGOAPDesires()
{
	return mGOAPDesires;
}

