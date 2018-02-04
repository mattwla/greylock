#ifndef GAME_MWMECHANICS_STATUSMANAGERIMP_H
#define GAME_MWMECHANICS_STATUSMANAGERIMP_H

#include "../mwbase/statusmanager.hpp"

#include "../mwworld/ptr.hpp"

#include <string>


namespace MWWorld
{
	class CellStore;
}

namespace MWStatus
{
	class StatusManager : public MWBase::StatusManager
	{

	public:
		StatusManager();
		
		virtual void initNpcStatus(std::string npcid);

		std::ifstream getCSV(std::string npcId);

		MWBase::Status stringToStatus(std::string statusstring);

		virtual bool hasStatus(MWWorld::Ptr ptr, MWBase::Status status);

		virtual void updatePtr(MWWorld::Ptr old, MWWorld::Ptr newptr);

	};
	
}

#endif
