#include "lifemanagerimp.hpp"
#include "../mwbase/tasksmanager.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>
#include <map>
#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwworld/cells.hpp"
#include "../mwworld/cellstore.hpp"


#include <limits.h>
#include <iostream>
#include <fstream>

GLLifeManager::LifeManager::LifeManager()
{
}

void GLLifeManager::LifeManager::newGame()
{
	
}

void GLLifeManager::LifeManager::update(float duration, bool paused)
{
	unsigned int itx = 0;
	while (itx < mLifeList.size())
	{
		MWBase::Life * currentLife = mLifeList[itx];
		if (inActiveRange(currentLife->mPtr))
		{
			currentLife->update(duration);
			//currentLife->mAwareness->refresh();
			//currentLife->mSubBrainsManager->calculate(currentLife->mAwareness);
		}
		itx++;
	}
}

void GLLifeManager::LifeManager::initialize()
{
	mLifeList.clear();
	mLifeList.shrink_to_fit();
	buildLifeList();
}

void GLLifeManager::LifeManager::buildLifeList() //starts on new game.... interesting. 
{
	std::string list = "schedules/npclist.csv";
	std::ifstream in(list.c_str());
	if (!in.is_open())
		std::cout << "Not open" << std::endl;
	else
		std::cout << "Open " << list << std::endl;
	std::string lifeid;
	while (getline(in, lifeid))
	{
		MWBase::Life* newlife = new MWBase::Life(lifeid);
		mLifeList.push_back(newlife);														
	}
}

bool GLLifeManager::LifeManager::inActiveRange(MWWorld::Ptr npc)
{
	bool inProcessingRange = false;
	//mwx fix me some bad redundency here against actors.cpp
	const float aiProcessingDistance = 7168;
	const float sqrAiProcessingDistance = aiProcessingDistance*aiProcessingDistance;
	MWWorld::Ptr player = MWBase::Environment::get().getWorld()->getPlayerPtr();
	if (npc.getCell()->isExterior())
	{
		inProcessingRange = (player.getRefData().getPosition().asVec3() - npc.getRefData().getPosition().asVec3()).length2() <= sqrAiProcessingDistance;
	}

	else
	{
		inProcessingRange = player.getCell() == npc.getCell();
	}
	//If player is resting, no one is in active range....
	if (inProcessingRange)
		inProcessingRange = !MWBase::Environment::get().getWindowManager()->getPlayerSleepingOrWaiting();
	return inProcessingRange;
}

