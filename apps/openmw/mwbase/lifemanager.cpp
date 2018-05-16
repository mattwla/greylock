#include "lifemanager.hpp"
#include <iostream>
#include "../mwworld/cellstore.hpp"
#include "../mwworld/cells.hpp" 

#include "../subbrains/subbrain.cpp"
#include "../subbrains/subbrainhunger.hpp"
#include "../subbrains/subbrainget.hpp"
#include "../subbrains/subbraininventory.hpp"
#include "../subbrains/subbrainusese.hpp"
#include "../subbrains/subbrainrelax.hpp"
#include "../subbrains/subbraincircadianrhythm.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../gllifemanager/goap.hpp"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include "../mwworld/class.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwclass/npc.hpp"

namespace MWBase
{


	//====================================LIFE OBJECT===========================================
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
		std::cout << "ERROR: NPC tried to request an item with status from inventory and there was nothing there" << std::endl;
		return nullptr;
	}

	void Life::say(std::string speech)
	{
		std::shared_ptr<Speech> speechobject = std::make_shared<Speech>(mPtr, speech);
		MWBase::Environment::get().getWindowManager()->createSpeech(speechobject);
		mCurrentSpeech = speechobject;
	}

	void Life::getDebugInfo()
	{
		std::cout << mId << std::endl;
		std::cout << "Hunger: " + std::to_string(mVitals.mHunger) << std::endl;
		std::cout << "Sleepiness: " + std::to_string(mVitals.mSleepiness) << std::endl;
		unsigned int ditx = 0;
		while (ditx < mDesireList.size())
		{
			std::cout << mDesireList[ditx]->debugInfo << std::endl;
			std::cout << mDesireList[ditx]->mValence << std::endl;
			ditx += 1;
		}
		if (mHasIntention)
		{
			std::cout << "intention info" << std::endl;
			unsigned int itx = 0;
			while (itx < mCurrentIntentionPlan.mGOAPNodeDataList.size())
			{
				std::cout << mCurrentIntentionPlan.mGOAPNodeDataList[itx]->mId << std::endl;
				itx++;
			}
			if (mHasQueuedIntention)
			{
				std::cout << "I have intention in queue" << std::endl;
			}
		}
		else
			std::cout << "has no intention" << std::endl;
	}

	void Life::update(float duration)
	{
		//If you were talking, and are now done talking, destroy the speech object
		if (mCurrentSpeech && mCurrentSpeech->mDone)
		{
			mCurrentSpeech = 0;
		}
		//change hunger, sleep, etc....
		metabolize(duration);
		//update objects npcs is aware of
		mAwareness->refresh();
		//pass awareness to subbrains, get back a list of desires. Subbrains push desires to life on their own, and maintain a pointer to them to update or remove them.
		mSubBrainsManager->calculate(mAwareness);

		//if we don't have a desire, just chill until we do. Wow how mindful. This will likely never happen. 
		if (mDesireList.size() == 0)
			return;
	
		//sort desires from most desired to least
		prioritizeDesires();

		//Run through desires from strongest to weakest until we find one we are already working on, or find one we can accomplish. I supose it is determineIntentionAndMakePlan()
		determineIntention();

		if (mHasQueuedIntention)
			runSwapIntentionPlan(duration);
		else if (mHasIntention)
			runTopIntentionPlan(duration);
	}

	void Life::submitDesirePtr(std::shared_ptr<MWBase::GOAPDesire> desire)
	{
		mDesireList.push_back(desire);
	}

	void Life::metabolize(float duration)
	{
		//mVitals.mHunger += duration / 150.0f;
		mVitals.mHunger += duration / 50.0f;
		if (mVitals.mHunger > 1000.0f)
		{
			std::cout << "npc starving" << std::endl;
			MWMechanics::CreatureStats& stats = mPtr.getClass().getCreatureStats(mPtr);
			MWMechanics::DynamicStat<float> health(mPtr.getClass().getCreatureStats(mPtr).getHealth());
			health.setCurrent(health.getCurrent() - 100.0f);
			stats.setHealth(health);
		}
		mVitals.mSleepiness += duration / 500.0f;
		//mVitals.mSleepiness += duration / 400.f;
	}

	void Life::prioritizeDesires()
	{
		std::sort(mDesireList.begin(), mDesireList.end(), [](const std::shared_ptr<GOAPDesire> d1, std::shared_ptr<GOAPDesire> d2) -> bool {return d1->mValence > d2->mValence; });
	}

	void Life::determineIntention()
	{
		bool foundPlan = false;
		bool continueIntention = false;
		unsigned int itx = 0;
		while (itx < mDesireList.size() && foundPlan != true)
		{
			//is this desire one we are currently working on? if so carry on. Else attempt to make a plan for this new desire.
			if (mDesireList[itx]->mIsIntention)
			{
				continueIntention = true;
				foundPlan = true;
			}
			else
			{
				IntentionPlan plan = selectIntentionPlan(mDesireList[itx]);
				foundPlan = plan.mPlanComplete;
				if (foundPlan)
				{
					//We found a new plan, do we already have an intention we need to stop?
					//MWX FIX ME THIS NEEDS REFACTORING
					if (mHasIntention)
					{
						mHasQueuedIntention = true;
						mQueuedIntentionPlan = plan;
						
					}
					else //had no plan before, so lets just start this one.
					{
						mCurrentIntentionPlan = plan;
						mHasIntention = true;
						
					}			
				}
			}
		itx += 1;
		}
	}

	IntentionPlan Life::selectIntentionPlan(std::shared_ptr<GOAPDesire> desire)
	{
		IntentionPlan newplan = mSubBrainsManager->createIntention(desire, mPtr);
		if (newplan.mPlanComplete) //were we able to find a plan that works?
		{
			//lets do it, let the desire know it is now an intention
			desire->mIsIntention = true;
			// set at last step, we go backwards
			newplan.mCurrentStep = newplan.mGOAPNodeDataList.size() - 1; 
		}
		return newplan;
	}
		
	void Life::runTopIntentionPlan(float duration)
	{
		if (mCurrentIntentionPlan.mCurrentBehaviorObject == 0) 	//Need to start an intention plan BO.
		{
			int step = mCurrentIntentionPlan.mCurrentStep;
			MWBase::GOAPNodeData * currentnode = mCurrentIntentionPlan.mGOAPNodeDataList[step].get();
			std::cout << "trying to:..." + currentnode->mId << std::endl;
			mCurrentIntentionPlan.mCurrentBehaviorObject = currentnode->getNewBehaviorObject(this, currentnode->mRefNum);
				//currentnode->mBehaviorObjectTemplate ->Clone(this, currentnode->mRefNum);
			mCurrentIntentionPlan.mCurrentBehaviorObject->start();
		}
		else //need to continue an intention plan BO
		{
			BehaviorObject * bo = mCurrentIntentionPlan.mCurrentBehaviorObject;
			MWBase::BOReturn status = bo->update(duration, mPtr);
			if (status == BOReturn::IN_PROGRESS)
			{
			}
			else if (status == BOReturn::COMPLETE)
			{
				delete bo;
				mCurrentIntentionPlan.mCurrentBehaviorObject = 0;
				mCurrentIntentionPlan.mCurrentStep -= 1;
				if (mCurrentIntentionPlan.mCurrentStep = -1)
				{
					//Whole plan done. Success.
					mHasIntention = false;
					mCurrentIntentionPlan.mDesire->mIsIntention = false;
				}
			}
			else if (status == BOReturn::FAILED)
			{
				//hack logic for now, delete the failed bo and get new intention.
				//In future, resubmit to planner for new way of accomplishing this node.
				delete bo;
				mHasIntention = false;
				mCurrentIntentionPlan.mDesire->mIsIntention = false;
				//3mCurrentIntentionPlan.mDesire->mIntentionPlan = false;
			}
		}
	}

	void Life::runSwapIntentionPlan(float duration)
	{
		//this condition doesn't really do anything
		if(!mSuccsessfulStopRequest)
			mSuccsessfulStopRequest = mCurrentIntentionPlan.stop(); //request current intention plan to srop
		
		//stop will eventually return a complete
		MWBase::BOReturn status = mCurrentIntentionPlan.mCurrentBehaviorObject->update(duration, mPtr);
		//if complete, mark the desire as no longer an intention, make the current intention plan the one we had waiting;
		if (status == COMPLETE)
		{
			mCurrentIntentionPlan.mDesire->mIsIntention = false;
			mCurrentIntentionPlan = mQueuedIntentionPlan;
			mHasQueuedIntention = false;
			mSuccsessfulStopRequest = false;
		}
	}

	//===================LIFE MANAGER ================================

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

	//===================================VITALS=================================================
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



	//================================INTENTION PLAN=============================================



	bool IntentionPlan::stop()
	{
		//request current BO to stop;
		//Return if BO said it was possible
		bool possible = mCurrentBehaviorObject->stop();
		return possible;
	}
}


//========================================SUBBRAINS MANAGER===============================================

void MWBase::SubBrainsManager::seperateCompletePlans(std::vector<IntentionPlan>& planlist, std::vector<IntentionPlan>& completelist, MWWorld::Ptr ptr)
{
	typedef std::vector<IntentionPlan> plans;
	plans newplanlist;

	for (plans::iterator it = planlist.begin(); it != planlist.end(); it++)
	{
		std::shared_ptr<GOAPNodeData> currentplan = it->mGOAPNodeDataList.back();
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
		case MWBase::HAS_ID_IN_INVENTORY:
			//evaluate inventory for specific id
			break;
		case MWBase::VITALS:
			//evaluate vitals
			break;
		case MWBase::HAS_OBJECT_STATUS_IN_INVENTORY:
			//evalualte if an object with a certain status is in inventory
			return hasObjectStatusInInventory(status, ptr);
			break;
		case MWBase::AWARE_OF_OBJECT_WITH_STATUS:
			//evaluate if npc has object in its awareness
			//return
			return hadObjectStatusInAwareness(status.mExtraData, ptr);
			break;
		case MWBase::STATUS_VOID: //no needed extra input
			return true;
	}
	
	std::cout << "status" + status.mExtraData + "not true" << std::endl;
	return false;
}


MWBase::IntentionPlan MWBase::SubBrainsManager::createIntention(std::shared_ptr<MWBase::GOAPDesire> desire, MWWorld::Ptr ptr)
{
	typedef std::vector<IntentionPlan> planlist;
	//plans in progress, not complete
	planlist possibleplans;
	//complete plans
	planlist completeplans;
	
	//A chain of nodes that need to be done to meet desire
	typedef std::vector<std::shared_ptr<GOAPNodeData>> nodechain;
	std::vector<nodechain> nodechainlist;

	//Seed the list, ask all subbrains if they have a behavior object that can meet our need.
	nodechain nc = querySubBrainsForGOAPMatches(desire->mStatus);
	for (nodechain::iterator itnc = nc.begin(); itnc != nc.end(); itnc++)
	{
		IntentionPlan plan;
		plan.mDesire = desire;
		plan.mGOAPNodeDataList.push_back(*itnc);
		possibleplans.push_back(plan);
	}

	//Now we have a list of actions NPC can take to meet desire, but some of those actions might not be possible yet.Seperate those we can do from those we need more planning to do
	if (possibleplans.size() > 0)
		seperateCompletePlans(possibleplans, completeplans, ptr);

	//Now we have a list of BOs that can meet our needs... but those BOs in turn might have needs that are not met.
	//Iterate through the list, and whenever a BO has more than one way of being met, copy the nodechain and push it back, with a new branch for each possible method
	while (possibleplans.size() > 0)
	{		
		//find all the ways we could possible be able to meet the requirements of starting this node
		nodechain possiblepaths = querySubBrainsForGOAPMatches(possibleplans[0].mGOAPNodeDataList.back()->mInputs[0]); //only checks first input, no support for multiple for inputs for now
		for (nodechain::iterator itb = possiblepaths.begin(); itb != possiblepaths.end(); itb++)
		{
			//copy plan
			IntentionPlan copiedplan = possibleplans[0];
			//add the new node we found to plan
			copiedplan.mGOAPNodeDataList.push_back(*itb);
			//push this new plan (not sure if complete yet) into our list of possible plan
			possibleplans.push_back(copiedplan);
		}

		//erase old plan, also serves to trim deadend plans
		possibleplans.erase(possibleplans.begin());

		//remove complete plans
		seperateCompletePlans(possibleplans, completeplans, ptr);
	}

	//Should I return lowest cost one? But what if we want an alternate plan? Maybe optional blacklist parameter for when a plan fails (pass the status we couldn't make work)
	if (completeplans.size() > 0)
		return completeplans[0];
	else
	{
		//used if we can't come up with a plan to meet this desire
		GOAPDesire emptydesire;
		IntentionPlan emptyplan(0);
		emptyplan.mPlanComplete = false;
		return emptyplan;
	}
}

std::vector<std::shared_ptr<MWBase::GOAPNodeData>> MWBase::SubBrainsManager::querySubBrainsForGOAPMatches(MWBase::GOAPStatus status)
{
	typedef std::vector<std::shared_ptr<MWBase::GOAPNodeData>> GOAPNodeDatalist;
	GOAPNodeDatalist result;

	for (std::vector<MWBase::SubBrain*>::iterator it = mSubBrains.begin(); it != mSubBrains.end(); ++it)
	{
		//ask the subbrain what behavior objects it has that can meet this status
		GOAPNodeDatalist nc = (*it)->getMatchingBehaviorObjects(status);
		//push them all into our result
		for (GOAPNodeDatalist::iterator itg = nc.begin(); itg != nc.end(); itg++)
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
	typedef std::unordered_map<ESM::RefNum, SensoryLink> linklist;
	MWBase::SensoryLinkStore * sensorystore = mLife->mAwareness->getSensoryLinkStore();
	linklist currentlinks = sensorystore->mSensoryLinks;
	for (linklist::iterator it = currentlinks.begin(); it != currentlinks.end(); it++)
	{
		if (it->second.mSEInstance->hasStatus(status))
		{
			return true;
		}
	}
	return false;
}

void MWBase::SubBrainsManager::calculate(MWBase::Awareness * awareness)
{
	for (std::vector<MWBase::SubBrain*>::iterator it = mSubBrains.begin(); it != mSubBrains.end(); ++it)
	{
		(*it)->calculate(awareness);
	
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
	sb = new SubBrainRelax(life);
	mSubBrains.push_back(sb);
	sb = new SubBrainUseSE(life);
	mSubBrains.push_back(sb);
	sb = new SubBrainCircadianRhythm(life);
	mSubBrains.push_back(sb);
}

std::vector<std::string> MWBase::SubBrainsManager::getSaveStates()
{
	std::vector<std::string> temp;
	return temp;
}
