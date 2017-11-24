#ifndef GAME_TASKS_GET_H
#define GAME_TASKS_GET_H

#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors


#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Get : public Task
	{
	private:

		bool pickupItem();

	public:

		Get();

		Get(MWWorld::Ptr dest);

		Get(std::string destId, std::string npcId);

		//Get(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp endtime);

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