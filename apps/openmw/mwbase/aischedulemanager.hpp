#ifndef GAME_MWBASE_AISCHEDULEMANAGER_H
#define GAME_MWBASE_AISCHEDULEMANAGER_H

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


#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>



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

namespace MWBase
{
    
    class AIScheduleManager
    {
            AIScheduleManager (const AIScheduleManager&);
            ///< not implemented

            AIScheduleManager& operator= (const AIScheduleManager&);
            ///< not implemented

        public:
			
		
			struct TaskPriorityPair //a task, a priority, and the global checks.
			{
				//mwx fis me could make in better place as now all task priority pairs hold this redundant map. mwx fix me
				std::map<std::string, MWTasks::Task::TypeID> taskStringToEnum =
				{
					{ "life", MWTasks::Task::TypeIDLife },
					{ "journey", MWTasks::Task::TypeIDJourney },
					{ "get", MWTasks::Task::TypeIDGet },
					{ "hunt", MWTasks::Task::TypeIDHunt },
					{ "sleep", MWTasks::Task::TypeIDSleep },
					{ "dance", MWTasks::Task::TypeIDDance },
					{ "pestle", MWTasks::Task::TypeIDPestle },
					{ "fish", MWTasks::Task::TypeIDFish },
					{ "sitground", MWTasks::Task::TypeIDSitground },
					{ "guard", MWTasks::Task::TypeIDGuard }


				};

				std::string npcId;
				MWTasks::Task::TypeID task;
				std::vector<std::string> mGlobals;
				int priority;


				TaskPriorityPair(std::string npcId, MWTasks::Task::TypeID task, int priority):
					npcId(npcId)
					, task(task)
					, priority(priority)
				{

				}

				TaskPriorityPair(std::string line)
				{
					//MWX fix me really should not be defining this here.
					typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;
					Tokenizer tok(line);
					int idx = 0;
					for (Tokenizer::iterator it(tok.begin()), end(tok.end()); it != end; ++it)
					{
						if (idx == 0) //first item is the task
						{
							task = taskStringToEnum[*it];
							idx += 1;
						}
						else //rest are conditions... for now
							mGlobals.push_back(*it);
						//vec.push_back(*it);
					}
					priority = 4;
				}
			};

			struct TimeBlock //All the things an NPC could be doing at a possible time. Generally first one that passes will be used, so order is important!
			{
				float mTime;
				std::vector<TaskPriorityPair*> mPossibleTasks;
				MWTasks::Task::TypeID getPossibleTask();
				bool checkScheduleGlobals(std::vector<std::string>);
			};

			struct Schedule
			{
				enum ScheduleParserExpecting {
					Time,
					Task
				};
				
				std::map<float, TimeBlock*> mTimeBlocks;

				Schedule(std::string npcId);

				std::ifstream getCSV(std::string npcId);

				MWTasks::Task::TypeID getScheduledTask();


			};
			AIScheduleManager() {}

            virtual ~AIScheduleManager() {}

			virtual bool checkScheduleGlobal(std::string global) = 0;

			virtual std::string getBed(std::string npcId) = 0;
			
      };



}

#endif
