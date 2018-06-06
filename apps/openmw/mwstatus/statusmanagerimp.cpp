#include "statusmanagerimp.hpp"

#include <limits.h>
#include <iostream>
#include <fstream>

#include <components/misc/rng.hpp>

#include <components/esm/esmwriter.hpp>
#include <components/esm/stolenitems.hpp>

#include <components/sceneutil/positionattitudetransform.hpp>

#include "../mwworld/esmstore.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/player.hpp"
#include "../mwworld/ptr.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/tasksmanager.hpp"

#include "../mwworld/timestamp.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/pathgrid.hpp"
#include "../mwstatus/statusobjects.hpp"

#include "../mwstatus/statusobjects.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>


std::unordered_map<MWBase::Status, MWBase::StatusObjectConstructor*, MWStatus::EnumClassHash> MWStatus::StatusManager::mStatusToConstructor;

namespace MWStatus
{
	void StatusManager::buildConstructorList()
	{


		auto c = MWBase::FloatStatusObject::getConstuctor();

		MWStatus::StatusManager::mStatusToConstructor[c->getStatusEnum()] = c;

		c = MWBase::OnFireStatusObject::getConstuctor();

		MWStatus::StatusManager::mStatusToConstructor[c->getStatusEnum()] = c;
	}


	StatusManager::StatusManager(MWBase::SmartEntityInstance * sei)
	{


		mSEI = sei;
	}



	void StatusManager::initNpcStatus(std::string npcid)
	{
		auto in = getCSV(npcid);
		MWBase::TasksManager::ZoneParserExpecting expecting = MWBase::TasksManager::Skip;
		//get tokenizer ready for seperating values by commas


		std::string line;
		std::vector<MWBase::Status> statusvec;

		//iterate through csv
		while (getline(in, line))
		{

			if (expecting == MWBase::TasksManager::Skip)
			{
				if (line != "STATUSSTART") //skip all lines until we get to the starting marker
					continue;
				else
				{
					expecting = MWBase::TasksManager::TaskName;
					continue;
				}
			}
			if (expecting == MWBase::TasksManager::TaskName)
			{
				statusvec.push_back(stringToStatus(line));
			}

		}
		
		//if(statusvec.size() > 0)
//			mStatusMap[MWBase::Environment::get().getWorld()->searchPtr(npcid, false)] = statusvec;
	}
	

	std::ifstream StatusManager::getCSV(std::string npcId)
	{
		std::string schedule = "schedules/" + npcId + ".csv";
		std::ifstream in(schedule.c_str());
		if (!in.is_open())
			std::cout << "Not open" << std::endl;
		else
			std::cout << "Open " << schedule << std::endl;

		return in;
	}

	MWBase::Status StatusManager::stringToStatus(std::string statusstring)
	{
		if (statusstring == "shaman")
			return MWBase::Shaman;
		else
			return MWBase::Shaman; //mwx broken

	}

	bool StatusManager::hasStatus(MWBase::Status status)
	{
		auto vec = mStatusMap;
		unsigned int idx = 0;
		while (idx < vec.size())
		{
			if (vec[idx] == status)
				return true;
			idx += 1;
		}

		return false;

	}

	//void StatusManager::updatePtr(MWWorld::Ptr old, MWWorld::Ptr newptr)
	//{
	//	auto vechold = mStatusMap[old];
	//	
	//	mStatusMap[newptr] = vechold;

	//	mStatusMap.erase(old);
	//}

	void StatusManager::giveStatus( MWBase::Status status)
	{
		//Do we already? have this status?
		auto vec = mStatusMap;
		unsigned int idx = 0;
		while (idx < vec.size())
		{
			if (vec[idx] == status)
				return;
			idx += 1;
		}

		//If we are here, we do not already have this status.

		//Do we have a special constructor for this statys?
		//If so, get a status object and let that take over. Else add the status to our statuslist.
		
		if (MWStatus::StatusManager::mStatusToConstructor.count(status))
		{
			auto constructor = mStatusToConstructor[status];
			auto object = constructor->getObject(mSEI);
			object->init();
			mStatusObjects.push_back(object);
		}
		else
		{
			mStatusMap.push_back(status);
		}
	}

	void StatusManager::removeStatus(MWBase::Status status)
	{
		auto vec = mStatusMap;
		unsigned int idx = 0;
		while (idx < vec.size())
		{
			if (vec[idx] == status)
			{
				mStatusMap.erase(mStatusMap.begin()+idx);
				return;
			}
			idx += 1;
		}
		
	}

	void StatusManager::update(float duration)
	{
		typedef std::vector<MWBase::StatusObject*> objectlist;
		objectlist newlist;

		for (objectlist::iterator it = mStatusObjects.begin(); it != mStatusObjects.end(); it++)
		{
			(*it)->update(duration);
			if (!(*it)->isDone())
			{
				newlist.push_back((*it));
			}

		}

		mStatusObjects = newlist;



	}

	void StatusManager::setStoredImpulse(float value)
	{
		mStoredImpulse = value;
	}

	float StatusManager::getStoredImpulse()
	{
		return mStoredImpulse;
	}

}
