#ifndef GAME_MWMECHANICS_STATUSMANAGERIMP_H
#define GAME_MWMECHANICS_STATUSMANAGERIMP_H

#include "../mwbase/statusmanager.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"

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

		MWBase::SmartEntityInstance * mSEI;


	public:
		StatusManager(MWBase::SmartEntityInstance * sei);
		
		virtual void initNpcStatus(std::string npcid);

		std::ifstream getCSV(std::string npcId);

		MWBase::Status stringToStatus(std::string statusstring);

		virtual bool hasStatus(MWBase::Status status);

		//virtual void updatePtr(MWWorld::Ptr old, MWWorld::Ptr newptr);

		virtual void giveStatus(MWBase::Status status);

		virtual void removeStatus(MWBase::Status status);

	};
	
}

#endif
