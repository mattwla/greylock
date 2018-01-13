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

		float mRange; //how close npc can get to goal before they stop, also plan on allowing them to stop somewhere in random radius.

		bool mHeadedToDoor;
		
	public:

		Journey();

		Journey(MWWorld::Ptr dest);

		Journey(std::string destId, std::string npcId, float range = 25.0f);
		
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

		std::string getBorderNodeId(std::string tnode1, std::string tnode2);

		bool init();

		bool mInitialized = false;

		std::string mDestId;

		bool mReadyForUpdate;
	
	};

}








#endif