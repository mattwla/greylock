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
				std::string npcId;
				std::string task;
				std::vector<std::string> mGlobals;
				int priority;
				TaskPriorityPair(std::string npcId, std::string task, int priority):
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
							task = *it;
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
				std::string getPossibleTask();
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

				std::string getScheduledTask();


			};


			

			//std::vector<Journey*> mActiveJourneys;

			//virtual void clearJourneys() = 0;
			
			
			AIScheduleManager() {}

            virtual ~AIScheduleManager() {}

			virtual std::ifstream fetchSchedule(std::string npcId) = 0;

			virtual std::string fetchCurrentScheduledTask(std::string npcId) = 0;
			
			virtual  std::vector<AIScheduleManager::TaskPriorityPair*> mapSchedule(std::vector<std::vector<std::string>> vecvec) = 0;

			virtual bool checkScheduleGlobal(std::string global) = 0;

			virtual void taskRouter(std::string npcID, std::string task, int priority) = 0;

			virtual void updateSchedules() = 0;

			//virtual void updateJourneys() = 0;

			virtual MWWorld::Ptr getHome(MWWorld::Ptr npc) = 0;

			virtual std::string getBed(std::string npcId) = 0;
			
			virtual bool goHome(MWWorld::Ptr npc) = 0;
			
			virtual bool goBar(MWWorld::Ptr npc) = 0;

			virtual bool goOutside(MWWorld::Ptr npc) = 0;

			virtual bool goBalmora(MWWorld::Ptr npc) = 0;

			virtual bool crossBalmora(MWWorld::Ptr npc) = 0;

			

		

			//virtual bool travel(MWWorld::Ptr npc, MWWorld::Ptr dest) = 0;

         

            enum ScheduleTask
            {
                ST_Home,
				ST_Bar
            };
            /**
             * @note victim may be empty
             * @param arg Depends on \a type, e.g. for Theft, the value of the item that was stolen.
             * @param victimAware Is the victim already aware of the crime?
             *                    If this parameter is false, it will be determined by a line-of-sight and awareness check.
             * @return was the crime seen?
             */
        
            enum PersuasionType
            {
                PT_Admire,
                PT_Intimidate,
                PT_Taunt,
                PT_Bribe10,
                PT_Bribe100,
                PT_Bribe1000
            };

			
      
    };

	//TaskPriorityPair * buildTaskPriorityPair(std::string line);

}

#endif
