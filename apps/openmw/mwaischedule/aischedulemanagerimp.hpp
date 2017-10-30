#ifndef GAME_MWMECHANICS_AISCHEDULEMANAGERIMP_H
#define GAME_MWMECHANICS_AISCHEDULEMANAGERIMP_H

#include "../mwbase/aischedulemanager.hpp"

#include "../mwworld/ptr.hpp"


namespace MWWorld
{
    class CellStore;
}

namespace MWAISchedule
{
    class AIScheduleManager : public MWBase::AIScheduleManager
    {
           

        public:

        

            AIScheduleManager();

          
			
			virtual std::ifstream fetchSchedule();

			virtual bool checkScheduleGlobal(std::string global);
			
			virtual  std::map<std::string, std::string> mapSchedule(std::vector<std::vector<std::string>> vecvec);
            ///< If the player is sleeping or waiting, this should be called every hour.
            /// @param sleep is the player sleeping or waiting?

			virtual void updateSchedules();

			virtual MWWorld::Ptr getHome(MWWorld::Ptr npc);


    };
}

#endif
