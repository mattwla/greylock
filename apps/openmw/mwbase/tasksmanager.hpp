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

		enum ZoneParserExpecting
		{
			TaskName,
			Zone,
			Skip
		};


		struct ZoneGlobalPair  //where an npc likes to do a given task, with the conditions the npc likes to do it under.
		{
			std::string mZoneId;
			std::vector<std::string> mGlobals;
		};

		struct ZoneAvailability
		{
			std::string mZoneId;
			int mZoneSlots;
			std::vector<bool> mAvailable;
		};
		TasksManager() {};

		virtual ~TasksManager() {}

		std::map<std::string, MWTasks::Task*>  mNpcMap; // maps NPC string id to a pointer to their life task.

		std::map<std::string, std::vector<ZoneGlobalPair*>> mZoneMap; //a map of tasks, linked to where NPC prefers to do each task.

		std::map<std::string, std::map<std::string, std::vector<ZoneGlobalPair*>> > mNpcIdToZones; //mwx fix me oh my god this is a data structure mess.

		virtual std::map<std::string, std::vector<ZoneGlobalPair*>> buildZoneMap(std::string npcId) = 0;
		
		std::map<std::string, ZoneAvailability*> mZoneAvailabilities;

		virtual int getZoneAvailability(std::string zoneId) = 0;

		virtual void freeZoneSlot(std::string zoneId, int idx) = 0;
		
		virtual std::string getZoneId(std::string npcId, std::string task) = 0;

		virtual void endLife(std::string npcId) = 0;

		virtual MWTasks::Task * taskEnumToTask(MWTasks::Task * lifetask, MWTasks::Task::TypeID task) = 0;
	
	};

}



#endif
