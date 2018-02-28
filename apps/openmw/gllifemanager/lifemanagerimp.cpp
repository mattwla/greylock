#include "lifemanagerimp.hpp"
#include "../mwbase/tasksmanager.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
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
	bool firstupdate = false;
	float hours = duration;
	int ticks = 0;
	const float min = 1.0 / 60.0; //define a minute
	if (mLastTimeReported == 0.0f) //our first update of current session, just log the time. mwx make this part of new game or load game process.
	{
		mLastTimeReported = hours;
		firstupdate = true;
		//return;
	}
	mTimePassed = hours - mLastTimeReported;
	if (mTimePassed < 0)
	{
		mTimePassed += 24.0f;
		
		//midnight timeboost
	}
	
	mLastTimeReported = hours;//remember the time for next update
	mTimeAccumulator += mTimePassed;//keep track of how much time has passed since we did last update

	if (mTimeAccumulator > min)
	{
		ticks += mTimeAccumulator / min; //How many minutes have passed?
		mTimeAccumulator -= ticks * min;
		
	}

	//process hours into 'ticks', for each tick poke our tasks. Tasks never have to worry about getting more than one tick in an update, ticks always delivered one at a time. Always = to 1 minute of activity.
	//GET EVERYONE A SCHEDULED TASK, OR MOVE ALONG THEIR CURRENT TASK

	//while (ticks > 0)
	bool updateSchedule = ticks > 5; //check for ->updated schedules<- every 5 minutes,.

	{
		int unsigned idx = 0;
		while (idx < mLifeList.size()) //iterate through all living things in world.
		{
			MWBase::Life* thislifeptr = mLifeList[idx];
		
			if (updateSchedule || firstupdate)
			{
				MWTasks::Task::TypeID scheduledtasktype = thislifeptr->mSchedule->getScheduledTask(); //figure out what this life thing is scheduled to do now through the AIScheduler (Big broad life tasks? work sleep play eat) Work can be commanded.

				if (!thislifeptr->mCurrentTask || thislifeptr->mCurrentTask->getTypeId() != scheduledtasktype)// if the life does not have a task, or its scheduled task has changed.
				{
					if (thislifeptr->mCurrentTask) //got a new task, still got an old task. Delete the whole task
						delete thislifeptr->mCurrentTask;

					MWTasks::Task* newtask = MWBase::Environment::get().getTasksManager()->taskEnumToTask(thislifeptr->mTaskChain, scheduledtasktype); //potential problem here, life manager right now only knows type of task, it knows nothing else like where the task should be.
					thislifeptr->mTaskChain->mSubTask = newtask; //task added to npcs current task chain (at head I presume?) mwx fix me memory leak here, what of old mSubTask?
					thislifeptr->mCurrentTask = newtask; //task also added to useful current task pointer, so life can access this task directly.

				}
			}


			if (thislifeptr->mSubTask) //is this npc futzing about with a task unrelated to the scheduled task? Assume it should be happening and update it.
			{
				thislifeptr->mPtr = thislifeptr->mSubTask->update(); //must set new ptr because tasks can cause outdated ptr (if npc changes cell...)
				if (thislifeptr->mSubTask->mDone) //is subtask over? Back to big broad life task (work, sleep, play, eat).
				{
					delete  thislifeptr->mSubTask;
					thislifeptr->mSubTask = 0;
					thislifeptr->mCurrentTask->resume(); //right now just pokes the currenttask part of taskchain.
				}
			}
			else
				thislifeptr->mPtr = thislifeptr->mTaskChain->update(); //no subtask? Update current life goal task
			
			idx += 1; //onto the next life
		}
		if (updateSchedule)
			ticks -= 5;
	}


	//SORT OUT REACTIONS HERE
	unsigned int reactionsidx = 0;
	while (reactionsidx < mLifeList.size())
	{
		MWBase::Life* currentlife = mLifeList[reactionsidx];
		if (inActiveRange(currentlife->mPtr)) //only update their reactions if they are in an active cell, no reactions for inactive ai now.
		{

			currentlife->mAwareOfList = MWBase::Environment::get().getAwarenessReactionsManager()->calculateAwareness(currentlife->mPtr);

			currentlife->mAvailableActions = MWBase::Environment::get().getAwarenessReactionsManager()->calculateReactions(currentlife->mPtr, *currentlife); //needs life object to get guard zone.... ridiculous mws fix me
			if (currentlife->mAvailableActions.size() > 0) //Are there possible ways to react to the environment?
			{
				currentlife->mSubTask = currentlife->mAvailableActions.begin()->first;
				//Just do first possible task found now.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        (mLifeList[reactionsidx]->mAvailableActions[0]
			}
		}
		reactionsidx += 1;
	}

	//Two loops here? Couldn't I just do one? Keep it like this for now but consider refactoring. Mwx fix me
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
		MWBase::Environment::get().getTasksManager()->buildZoneMap(lifeid);
		//mwx fix me should make all this logic a discrete newgame thingy, right now is very tied up in a weird way															
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