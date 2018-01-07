#ifndef GAME_TASKS_SLEEP_H
#define GAME_TASKS_SLEEP_H

#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors


#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Sleep : public Task
	{
	private:


	public:

		Sleep();

		Sleep(MWWorld::Ptr dest);

		Sleep(std::string destId, std::string npcId);

		//Sleep(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp endtime);

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