#ifndef GAME_MWBASE_TASKSMANAGER_H
#define GAME_MWBASE_TASKSMANAGER_H

#include <string>
#include <vector>
#include <list>
#include <set>
#include <stdint.h>
#include <map>

#include <list>

#include <components/esm/loadpgrd.hpp>
#include "../mwmechanics/pathgrid.hpp"
#include "../mwworld/timestamp.hpp"

#include "../mwworld/ptr.hpp"
#include "../mwtasks/task.hpp"


namespace osg
{
	class Vec3f;
}

namespace ESM
{
	struct Class;

	class ESMReader;
	class ESMWriter;
}

namespace MWWorld
{
	class Ptr;
	class CellStore;
	class CellRef;
}

namespace Loading
{
	class Listener;
}

namespace MWTasks
{
	class Task;
}

namespace MWBase
{
	class TasksManager
	{
		TasksManager(const TasksManager&);
	///< not implemented

	TasksManager& operator= (const TasksManager&);
	///< not implemented
	
	public:

	
		TasksManager() {};

		virtual ~TasksManager() {}

		std::map<std::string, MWTasks::Task*>  mNpcMap;

		virtual void update() = 0;

		virtual MWTasks::Task* getScheduledTask(std::string npcId) = 0;


	
	
	};


}



#endif
