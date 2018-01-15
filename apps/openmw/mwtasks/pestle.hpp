#ifndef GAME_TASKS_PESTLE_H
#define GAME_TASKS_PESTLE_H

#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors


#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Pestle : public Task
	{
	private:


	public:

		Pestle();

		Pestle(MWWorld::Ptr dest);

		Pestle(std::string destId, std::string npcId);

		~Pestle();

		//Pestle(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp endtime);

		unsigned int mStep;

		MWBase::TravelNodesManager * mTravelNodesManager = MWBase::Environment::get().getTravelNodesManager();

		std::vector<int> mTravelNodeItinerary;

		MWWorld::Ptr mDestination;

		MWWorld::TimeStamp mStartTime;

		virtual void update();

		std::string mZoneId;

		int mZoneSlotIdx;
	

		virtual int getTypeId() const;

		bool init();

		bool mInitialized = false;

		std::string mDestId;

		bool mReadyForUpdate;

	};

}








#endif