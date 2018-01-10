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

		std::map<std::string, MWTasks::Task*> buildNpcMap();


		float mLastTimeReported;

		float mTimePassed;

		float mTimeAccumulator;

	public:

		TasksManager();

		virtual void update(float hours, bool incremental = false);

		virtual Task * getScheduledTask(std::string npcId, std::string stask);

		//virtual Task* getScheduledTask(std::string npcId);

		bool isInActiveRange(std::string npcId);

	};
}


#endif