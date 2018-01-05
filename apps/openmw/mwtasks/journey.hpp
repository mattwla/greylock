#ifndef GAME_TASKS_JOURNEY_H
#define GAME_TASKS_JOURNEY_H

#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors


#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Journey : public Task
	{
		
		bool mWasActiveLastUpdate;

		int mTickCount;
		
	public:

		Journey();

		Journey(MWWorld::Ptr dest);

		Journey(std::string destId, std::string npcId);
		
		//Journey(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp endtime);

		unsigned int mStep;
		
		MWBase::TravelNodesManager * mTravelNodesManager = MWBase::Environment::get().getTravelNodesManager();
		
		std::vector<int> mTravelNodeItinerary;
		
		MWWorld::Ptr mDestination;
		
		MWWorld::TimeStamp mStartTime;

		virtual void update();

		void leftActiveCells();

		bool hasArrived();

		void inactiveUpdate();

		virtual int getTypeId() const;

		bool init();

		bool mInitialized = false;

		std::string mDestId;

		bool mReadyForUpdate;
	
	};

}








#endif