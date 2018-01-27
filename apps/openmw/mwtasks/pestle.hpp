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

		
		Pestle(MWTasks::Task* lifetask);

		~Pestle();

		//Pestle(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp endtime);

		unsigned int mStep;

		virtual MWWorld::Ptr update();

		std::string mZoneId;

		int mZoneSlotIdx;
	
		virtual int getTypeId() const;

		std::string mDestId;


	};

}








#endif