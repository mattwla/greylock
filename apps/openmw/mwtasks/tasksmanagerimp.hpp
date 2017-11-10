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

	public:

		TasksManager();

		virtual void update();

		virtual Task* getScheduledTask(std::string npcId);

	};
}


#endif