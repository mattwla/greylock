#include "lifemanagerimp.hpp"
#include "../mwbase/tasksmanager.hpp"
#include "../mwbase/windowmanager.hpp"
#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>
#include <map>

#include <limits.h>
#include <iostream>
#include <fstream>

GLLifeManager::LifeManager::LifeManager()
{
}

void GLLifeManager::LifeManager::update(float duration, bool paused)
{

	int unsigned idx = 0;
	while (idx < mLifeList.size())
	{
		auto task = mLifeList[idx]->mSchedule->getScheduledTask();
	
		if (!mLifeList[idx]->mCurrentTask || mLifeList[idx]->mCurrentTask->getTypeId() != task )//|| task == MWTasks::Task::TypeIDGet) // get means no task.
		{
			auto newtask = MWBase::Environment::get().getTasksManager()->taskEnumToTask(mLifeList[idx]->mTaskChain, task);
			mLifeList[idx]->mTaskChain->mSubTask = newtask;
			mLifeList[idx]->mCurrentTask = newtask;
		}

		mLifeList[idx]->mTaskChain->update();
		idx += 1;
	}
	

	//MWBase::Environment::get().getTasksManager()->update(duration, paused);


}

void GLLifeManager::LifeManager::initialize()
{
	mLifeList.clear();
	mLifeList.shrink_to_fit();
	buildLifeList();
}

void GLLifeManager::LifeManager::buildLifeList()
{

	//auto sm = MWBase::Environment::get().getStatusManager();

	//std::map<MWB npcmap;

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
		//mNpcIdToZones[npc] = 
		MWBase::Environment::get().getTasksManager()->buildZoneMap(lifeid);
		//MWBase::Environment::get().getStatusManager()->initNpcStatus(npc); //mwx fix me should make all this logic a discrete newgame thingy, right now is very tied up in a weird way
																		   //CREATE AND ASSIGN NEW LIFE TASK HERE MWX
	}

	//Give schedule manager
	//happens in life constructor

	//Give task manager
	//happens in life constructor

	//Give awareness reactions manager
	//Can get list of possible reactions and list of aware of items

	//Give zone manager


	//Give status manager

}

bool GLLifeManager::LifeManager::inActiveRange(MWWorld::Ptr npc)
{
	bool inProcessingRange;
	//mwx fix me some bad redundency here against actors.cpp
	const float aiProcessingDistance = 7168;

	const float sqrAiProcessingDistance = aiProcessingDistance*aiProcessingDistance;
	MWWorld::Ptr player = MWBase::Environment::get().getWorld()->getPlayerPtr();
	//MWWorld::Ptr npc = MWBase::Environment::get().getWorld()->searchPtr(npcId, false);
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

	/*	if (npc.getCellRef().getRefId() == "eldertimothy")
	{
	std::cout << "breaking here for study" << std::endl;
	}*/
	return inProcessingRange;

}