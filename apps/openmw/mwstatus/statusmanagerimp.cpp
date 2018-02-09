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

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>


namespace MWStatus
{
	StatusManager::StatusManager()
	{

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
			mStatusMap[MWBase::Environment::get().getWorld()->searchPtr(npcid, false)] = statusvec;
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

	}

	bool StatusManager::hasStatus(MWWorld::Ptr ptr, MWBase::Status status)
	{
		auto vec = mStatusMap[ptr];
		unsigned int idx = 0;
		while (idx < vec.size())
		{
			if (vec[idx] == status)
				return true;
			idx += 1;
		}

		return false;

	}

	void StatusManager::updatePtr(MWWorld::Ptr old, MWWorld::Ptr newptr)
	{
		auto vechold = mStatusMap[old];
		
		mStatusMap[newptr] = vechold;

		mStatusMap.erase(old);
	}

	void StatusManager::giveStatus(MWWorld::Ptr ptr, MWBase::Status status)
	{
		auto vec = mStatusMap[ptr];
		unsigned int idx = 0;
		while (idx < vec.size())
		{
			if (vec[idx] == status)
				return;
			idx += 1;
		}
		mStatusMap[ptr].push_back(status);
		//return false;
	}

	void StatusManager::removeStatus(MWWorld::Ptr ptr, MWBase::Status status)
	{
		auto vec = mStatusMap[ptr];
		unsigned int idx = 0;
		while (idx < vec.size())
		{
			if (vec[idx] == status)
			{
				mStatusMap[ptr].erase(mStatusMap[ptr].begin()+idx);
				return;
			}
			idx += 1;
		}
		//mStatusMap[ptr].push_back(status);
	}

}
