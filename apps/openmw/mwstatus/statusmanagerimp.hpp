#ifndef GAME_MWMECHANICS_STATUSMANAGERIMP_H
#define GAME_MWMECHANICS_STATUSMANAGERIMP_H

#include "../mwbase/statusmanager.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwstatus/statusobjects.hpp"
#include <memory>
#include <map>
#include <unordered_map>

#include "../mwworld/ptr.hpp"

#include <string>


namespace MWWorld
{
	class CellStore;
}

namespace MWStatus
{



	struct EnumClassHash
	{
		template <typename T>
		std::size_t operator()(T t) const
		{
			return static_cast<std::size_t>(t);
		}
	};


	class StatusManager : public MWBase::StatusManager
	{

		

		MWBase::SmartEntityInstance * mSEI;
		std::vector<MWBase::StatusObject*> mStatusObjects;
		float mStoredImpulse = 0.0f;


	public:

		static std::unordered_map<MWBase::Status, MWBase::StatusObjectConstructor*, EnumClassHash> mStatusToConstructor;

		static void buildConstructorList();

		StatusManager(MWBase::SmartEntityInstance * sei);
		
		virtual void initNpcStatus(std::string npcid);

		std::ifstream getCSV(std::string npcId);

		MWBase::Status stringToStatus(std::string statusstring);

		virtual bool hasStatus(MWBase::Status status);

		//virtual void updatePtr(MWWorld::Ptr old, MWWorld::Ptr newptr);

		virtual void giveStatus(MWBase::Status status);

		virtual void removeStatus(MWBase::Status status);

		virtual void update(float duration);

		virtual void setStoredImpulse(float value);

		virtual float getStoredImpulse();

	};
	
}

#endif
