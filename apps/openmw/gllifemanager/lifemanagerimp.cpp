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







bool GLLifeManager::LifeManager::inActiveRange(MWWorld::Ptr npc)
{
	if (MWBase::Environment::get().getWorld()->getPlayerPtr() == npc)
		return false;
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


