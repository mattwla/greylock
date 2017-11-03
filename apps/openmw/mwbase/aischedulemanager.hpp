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

#include "../mwworld/ptr.hpp"

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

			struct TravelNode
			{
				//ALL SUBJECT TO CHANGE

				int id; //An id, static, must be used for astar to function. (use to make a map?)
				MWWorld::Ptr marker; //An in game location, for spawning NPCs
				ESM::Pathgrid::Point point; //A point for astar pathfinding
				std::vector<MWWorld::Ptr> npcs; //A list of npcs at node
			};

			std::map<int, TravelNode>  travelNodeMap;
			
			
			AIScheduleManager() {}

            virtual ~AIScheduleManager() {}

			virtual std::ifstream fetchSchedule() = 0;
			
			virtual  std::map<std::string, std::string> mapSchedule(std::vector<std::vector<std::string>> vecvec) = 0;

			virtual bool checkScheduleGlobal(std::string global) = 0;

			virtual void updateSchedules() = 0;

			virtual MWWorld::Ptr getHome(MWWorld::Ptr npc) = 0;
			
			virtual bool goHome(MWWorld::Ptr npc) = 0;
			
			virtual bool goBar(MWWorld::Ptr npc) = 0;

			virtual bool goOutside(MWWorld::Ptr npc) = 0;

			virtual bool goBalmora(MWWorld::Ptr npc) = 0;

			virtual bool crossBalmora(MWWorld::Ptr npc) = 0;

			virtual std::map<int, TravelNode> buildTravelNodes() = 0;

         

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
}

#endif