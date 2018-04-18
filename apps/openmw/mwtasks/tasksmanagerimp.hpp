#ifndef GAME_MWMECHANICS_TASKSMANAGERIMP_H
#define GAME_MWMECHANICS_TASKSMANAGERIMP_H

#include "../mwbase/tasksmanager.hpp"
#include "../mwtasks/journey.hpp"
#include "../mwbase/lifemanager.hpp"

#include "../mwworld/ptr.hpp"

#include <map>

namespace MWWorld
{
	class CellStore;
}

namespace MWTasks
{
	class TasksManager : public MWBase::TasksManager
	{

	private:

		virtual std::map<std::string, std::vector<ZoneGlobalPair*>> buildZoneMap(std::string npcId);

		virtual int getZoneAvailability(std::string zoneId);

		virtual void freeZoneSlot(std::string zoneId, int idx);

		void buildZoneAvailabilities();

		std::ifstream getCSV(std::string npcId);

		float mLastTimeReported;

		float mTimePassed;

		float mTimeAccumulator;


	public:

		TasksManager();

		virtual Task * taskEnumToTask(MWTasks::Task * lifetask, MWTasks::Task::TypeID task);

		virtual std::string getZoneId(std::string npcId, std::string task);

		virtual void endLife(std::string npcId);

	};


	

	//class JourneyManager : public MWBase::JourneyManager
	//{

	//	private:

	//		MWBase::Life * mOwnerLife;
	//		int mCurrentJourneyTicket = 0;
	//	
	//	public:
	//		JourneyManager::JourneyManager(MWBase::Life * life)
	//		{
	//			mOwnerLife = life;

	//		}

	//		virtual bool requestNewJourney();

	//		virtual MWBase::JourneyStatus updateJourney(int ticket);

	//};
}


#endif