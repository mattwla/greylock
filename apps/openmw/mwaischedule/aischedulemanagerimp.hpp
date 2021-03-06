#ifndef GAME_MWMECHANICS_AISCHEDULEMANAGERIMP_H
#define GAME_MWMECHANICS_AISCHEDULEMANAGERIMP_H

#include "../mwbase/aischedulemanager.hpp"

#include "../mwworld/ptr.hpp"

#include <string>


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

			virtual bool checkScheduleGlobal(std::string global);

			virtual std::string getBed(std::string npcId);

    };
}

#endif
