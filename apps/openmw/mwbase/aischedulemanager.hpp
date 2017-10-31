#ifndef GAME_MWBASE_AISCHEDULEMANAGER_H
#define GAME_MWBASE_AISCHEDULEMANAGER_H

#include <string>
#include <vector>
#include <list>
#include <set>
#include <stdint.h>
#include <map>

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
    /// \brief Interface for game mechanics manager (implemented in MWMechanics)
    class AIScheduleManager
    {
            AIScheduleManager (const AIScheduleManager&);
            ///< not implemented

            AIScheduleManager& operator= (const AIScheduleManager&);
            ///< not implemented

        public:

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
