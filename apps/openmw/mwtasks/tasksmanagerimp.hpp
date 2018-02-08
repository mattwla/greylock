#ifndef GAME_MWMECHANICS_TASKSMANAGERIMP_H
#define GAME_MWMECHANICS_TASKSMANAGERIMP_H

#include "../mwbase/tasksmanager.hpp"
#include "../mwtasks/journey.hpp"

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

	

	
		//std::map<std::string, MWTasks::Task*> buildNpcMap();

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

		//virtual void forceUpdate();

		virtual void update(float hours, bool incremental = false);

		virtual Task * taskEnumToTask(MWTasks::Task * lifetask, MWTasks::Task::TypeID task);

		//virtual Task * getScheduledTask(MWTasks::Task* lifetask, MWTasks::Task::TypeID task);

		//virtual Task * getScheduledTask(std::string npcId, MWTasks::Task::TypeID task);

		//virtual Task * getScheduledTask(std::string npcId, std::string stask);

		//virtual Task* getScheduledTask(std::string npcId);

		//bool isInActiveRange(MWWorld::Ptr);

		virtual std::string getZoneId(std::string npcId, std::string task);

		virtual void endLife(std::string npcId);

	};
}


#endif