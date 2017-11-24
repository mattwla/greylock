#ifndef GAME_TASKS_HUNT_H
#define GAME_TASKS_HUNT_H

#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors


#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Hunt : public Task
	{
	private:

	

	public:

		Hunt();

		Hunt(MWWorld::Ptr dest);

		Hunt(std::string destId, std::string npcId);

		//Hunt(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp endtime);

		unsigned int mStep;

		MWBase::TravelNodesManager * mTravelNodesManager = MWBase::Environment::get().getTravelNodesManager();

		std::vector<int> mTravelNodeItinerary;

		MWWorld::Ptr mDestination;

		MWWorld::TimeStamp mStartTime;

		virtual void update();



		virtual int getTypeId() const;

		bool init();

		bool mInitialized = false;

		std::string mDestId;

		bool mReadyForUpdate;

	};

}








#endif