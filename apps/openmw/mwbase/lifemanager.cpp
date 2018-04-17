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

	//LIFE

	void Life::getDebugInfo()
	{
		std::cout << mId << std::endl;
		std::cout << std::to_string(mRefNum) << std::endl;
		std::cout << "Hunger: " + std::to_string(mVitals.mHunger) << std::endl;
		std::cout << "Sleepiness: " + std::to_string(mVitals.mSleepiness) << std::endl;
		mSubBrainsManager->logDesires();
		mSubBrainsManager->logWorldstate();
		//std::cout << "original cell: " + mOwnerCell->getCell  << std::endl;
	}

	void Life::update(float duration)
	{
		metabolize(duration);
		mAwareness->refresh();
		mSubBrainsManager->calculate(mAwareness);
		//std::vector<BehaviorObject*> desires = mSubBrainsManager->getDesires();
		
		std::vector<GOAPDesire> GOAPDesires = mSubBrainsManager->getGOAPDesires();
		if (GOAPDesires.size() > 0)
		{
			prioritizeDesires(GOAPDesires);
			//mSubBrainsManager->evaluateGOAPStatus(GOAPDesires[0].mStatus, mPtr);
			mSubBrainsManager->createIntention(GOAPDesires[0].mStatus, mPtr);
		}
		

		//std::vector<BehaviorObject*> prioritizedDesires = prioritizeDesires(desires);
		
		//std::vector<WorldstateAtom> worldstate = mSubBrainsManager->getWorldstate();
		//extract desires, put into desireprioritizer

		//std::vector<GOAPData*> GOAPNodes = mSubBrainsManager->getGOAPNodes();
		//extract BONodes, put into GOAP
		
	}

	void Life::metabolize(float duration)
	{
		mVitals.mHunger += duration / 300.0f;
		mVitals.mSleepiness += duration / 2000.f;
	}

	std::vector<BehaviorObject*> Life::prioritizeDesires(std::vector<BehaviorObject*> desires)
	{
		return std::vector<BehaviorObject*>();
	}

	void Life::prioritizeDesires(std::vector<GOAPDesire>& desires)
	{
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
		GOAPData * currentplan = it->mGOAPDataList.back();
		bool plancomplete = evaluateGOAPStatus(currentplan->mInputs[0], ptr); //currently only checks 1 status;
		if (plancomplete)
		{
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
	}
	
	std::cout << "status" + status.mExtraData + "not true" << std::endl;
	return false;
}

bool MWBase::SubBrainsManager::createIntention(MWBase::GOAPStatus status, MWWorld::Ptr ptr)
{
	std::cout << "attempting to create intention" << std::endl;
	typedef std::vector<IntentionPlan> planlist;
	planlist possibleplans;
	planlist completeplans;

	typedef std::vector<GOAPData*> nodechain;
	std::vector<nodechain> nodechainlist;
	
	//Seed the list, ask all subbrains if they have a behavior object that can meet our need.
	nodechain nc = querySubBrainsForGOAPMatches(status);
	for (nodechain::iterator itnc = nc.begin(); itnc != nc.end(); itnc++)
	{
		IntentionPlan plan;
		plan.mGOAPDataList.push_back(*itnc);
	}


	if (possibleplans.size() == 0)
	{
		std::cout << "could not start plan to meet goal: " + status.mExtraData << std::endl;
	}

	seperateCompletePlans(possibleplans, completeplans, ptr);

	//Now we have a list of BOs that can meet our needs... but those BOs in turn might have needs that are not met.
	//Iterate through the list, and whenever a BO has more than one way of being met, copy the nodechain and push it back, with a new branch for each possible method

	

	while (possibleplans.size() > 0)
	{
		
		nodechain possiblepaths = querySubBrainsForGOAPMatches(possibleplans[0].mGOAPDataList.back()->mInputs[0]); //only checks first
		if (possiblepaths.size() == 0)
		{
			std::cout << "dead end plan" << std::endl;
			std::cout << possibleplans[0].mDesiredState.mExtraData << std::endl;
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
	
	return false;
}

std::vector<MWBase::GOAPData*> MWBase::SubBrainsManager::querySubBrainsForGOAPMatches(MWBase::GOAPStatus status)
{
	typedef std::vector<MWBase::GOAPData*> goapdatalist;
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
			auto sei = sem->getSmartEntityInstance(*it);
			if (sei->hasStatus(status.mExtraData))
				return true;
			
		}
	}
	return false;
}

void MWBase::SubBrainsManager::calculate(MWBase::Awareness * awareness)
{
	mDesires.clear();
	mGOAPNodes.clear();
	mWorldState.clear();

	for (std::vector<MWBase::SubBrain*>::iterator it = mSubBrains.begin(); it != mSubBrains.end(); ++it)
	{
		
		typedef std::vector<MWBase::BehaviorObject*> bolist;
		(*it)->calculate(awareness);
		
		//collect GOAPNodes from subbrain, add to list.
		std::vector<GOAPData*> goapnodes = (*it)->getGOAPNodes();
		for (std::vector<GOAPData*>::iterator itg = goapnodes.begin(); itg != goapnodes.end(); itg++)
		{
			mGOAPNodes.push_back(*itg);
		}
		


		//Collect desires from subbrain, add to list.
		bolist desirelist = (*it)->getDesires();
		for (bolist::iterator itb = desirelist.begin(); itb != desirelist.end(); itb++)
		{
			mDesires.push_back(*itb);
		}

		std::vector<MWBase::GOAPDesire> gdesirelist = (*it)->getGOAPDesires();
		for (std::vector<MWBase::GOAPDesire>::iterator gdl = gdesirelist.begin(); gdl != gdesirelist.end(); gdl++)
		{
			mGOAPDesires.push_back(*gdl);
		}

		//Collect worldstate from subbrain, add to list.
		std::vector<WorldstateAtom> worldstate = (*it)->getWorldstate();
		for (std::vector<WorldstateAtom>::iterator itw = worldstate.begin(); itw != worldstate.end(); itw++)
		{
			mWorldState.push_back(*itw);
		}

	
	}

}

MWBase::SubBrainsManager::SubBrainsManager(MWBase::Life * life)
{
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

	if (mDesires.size() == 0)
	{
		std::cout << "no desires" << std::endl;
		return;
	}
	std::cout << "desires:" << std::endl;
	typedef std::vector<MWBase::BehaviorObject*> desirelist;
	for (desirelist::iterator it = mDesires.begin(); it != mDesires.end(); it++)
	{
		(*it)->getDebugInfo();
	}

}

void MWBase::SubBrainsManager::logWorldstate()
{

	if (mWorldState.size() == 0)
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
	}

}

std::vector<MWBase::BehaviorObject*> MWBase::SubBrainsManager::getDesires()
{
	return mDesires;
}

std::vector<MWBase::GOAPDesire> MWBase::SubBrainsManager::getGOAPDesires()
{
	return mGOAPDesires;
}

std::vector<WorldstateAtom> MWBase::SubBrainsManager::getWorldstate()
{
	return std::vector<WorldstateAtom>();
}


