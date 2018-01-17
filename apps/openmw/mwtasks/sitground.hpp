#ifndef GAME_TASKS_SITGROUND_H
#define GAME_TASKS_SITGROUND_H

#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors


#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Sitground : public Task
	{
	private:

		bool pickupItem();

	public:

		Sitground();

		Sitground(MWWorld::Ptr dest);

		Sitground(std::string destId, std::string npcId);

		~Sitground();

		//Sitground(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp endtime);

		unsigned int mStep;

		MWBase::TravelNodesManager * mTravelNodesManager = MWBase::Environment::get().getTravelNodesManager();

		std::vector<int> mTravelNodeItinerary;

		MWWorld::Ptr mDestination;

		MWWorld::TimeStamp mStartTime;

		virtual void update();

		int mZoneSlotIdx;

		std::string mZoneId;

		virtual int getTypeId() const;

		bool init();

		bool mInitialized = false;

		std::string mDestId;

		bool mReadyForUpdate;

	};

}








#endif