#ifndef GAME_TASKS_JOURNEY_H
#define GAME_TASKS_JOURNEY_H

#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors

#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Journey : Task
	{

		Journey(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp endtime);
		Journey(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, std::string task);
		std::string mNpcId;
		unsigned int mStep;
		std::vector<int> mTravelNodeItinerary;
		MWWorld::Ptr mDestination;
		std::string mOnCompleteTask;
		MWWorld::TimeStamp mStartTime;

		void update();
		bool readyForUpdate();
	};

}








#endif