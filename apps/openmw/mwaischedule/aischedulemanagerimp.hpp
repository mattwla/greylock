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

			virtual bool travel(MWWorld::Ptr npc, MWWorld::Ptr dest);

			virtual bool goHome(MWWorld::Ptr npc);

			virtual bool goBar(MWWorld::Ptr npc);

			virtual bool goOutside(MWWorld::Ptr npc);

			virtual bool goBalmora(MWWorld::Ptr npc);

			virtual bool crossBalmora(MWWorld::Ptr npc);
			
			virtual std::map<int, TravelNode> buildTravelNodes();

			virtual void buildPathGrid(ESM::Pathgrid *grid);

			//virtual ESM::Pathgrid buildPathGrid();



    };
}

#endif
