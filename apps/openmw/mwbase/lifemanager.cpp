#include "lifemanager.hpp"
#include <iostream>
#include "../mwworld/cellstore.hpp"
#include "../mwworld/cells.hpp" 
#include "../subbrains/subbrain.hpp"
#include "../subbrains/subbrainhunger.hpp"
#include "../subbrains/subbrainget.hpp"
#include "../subbrains/subbraininventory.hpp"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>


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
		//std::cout << "original cell: " + mOwnerCell->getCell  << std::endl;
	}

	void Life::update(float duration)
	{
		metabolize(duration);
		mAwareness->refresh();
		mSubBrainsManager->calculate(mAwareness);
	}

	void Life::metabolize(float duration)
	{
		mVitals.mHunger += duration / 1000.0f;
		mVitals.mSleepiness += duration / 2000.f;
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




void MWBase::SubBrainsManager::calculate(MWBase::Awareness * awareness)
{
	
	//MWBase::SensoryLinkStore * store = awareness->getSensoryLinkStore();

	for (std::vector<MWBase::SubBrain*>::iterator it = mSubBrains.begin(); it != mSubBrains.end(); ++it)
	{
		
		typedef std::vector<MWBase::BehaviorObject*> bolist;
		(*it)->calculate(awareness);
		bolist desirelist = (*it)->getDesires();
		if (mDesires.size() == 0) //tempory to prevent bloating.
		{
			for (bolist::iterator itb = desirelist.begin(); itb != desirelist.end(); itb++)
			{
				mDesires.push_back(*itb);
			}
		}
		//mDesires.push_back()
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


