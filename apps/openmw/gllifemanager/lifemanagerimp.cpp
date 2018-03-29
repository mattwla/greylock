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

void MWBase::LifeManager::newGame()
{

	/*auto mActiveCells = MWBase::Environment::get().getWorld()->getCells();
	const MWWorld::Scene::CellStoreCollection& collection = mActiveCells;
	for (MWWorld::Scene::CellStoreCollection::const_iterator cellIt = collection.begin(); cellIt != collection.end(); ++cellIt)
	{
		MWWorld::ListObjectsVisitor visitor;
		(*cellIt)->forEach(visitor);

		for (std::vector<MWWorld::Ptr>::iterator it = visitor.mObjects.begin(); it != visitor.mObjects.end(); ++it)
		{
			std::string type = it->getClass().getTypeName();
			if (type == typeid(ESM::NPC).name())
			{
				MWBase::Life* newlife = new MWBase::Life(it->getCellRef().getRefId);
				mLifeList.push_back(newlife);
			}
		}
	}*/
}
void GLLifeManager::LifeManager::update(float duration, bool paused)
{
	unsigned int itx = 0;
	while (itx < mLifeList.size())
	{
	
		
		MWBase::Life * currentLife = mLifeList[itx];
		if (inActiveRange(currentLife->mPtr))
		{
			currentLife->mAwareness->refresh();
			
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
		
		//MWBase::Environment::get().getTasksManager()->buildZoneMap(lifeid);
		//mwx fix me should make all this logic a discrete newgame thingy, right now is very tied up in a weird way															
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
	
	//return true;
}