#ifndef GAME_TASKS_JOURNEY_H
#define GAME_TASKS_JOURNEY_H

#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors

#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Journey : public Task
	{
		
		
	public:

		Journey();
		Journey(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp endtime);

		unsigned int mStep;
		std::vector<int> mTravelNodeItinerary;
		MWWorld::Ptr mDestination;
		
		MWWorld::TimeStamp mStartTime;

		void update();
	
	};

}








#endif