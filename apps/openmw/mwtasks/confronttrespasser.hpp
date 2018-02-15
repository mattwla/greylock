#ifndef GAME_TASKS_CONFRONTTRESPASSER_H
#define GAME_TASKS_CONFRONTTRESPASSER_H


#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors


#include "../mwbase/world.hpp"

namespace MWTasks
{

	class ConfrontTrespasser : public Task
	{
	private:


	public:

		ConfrontTrespasser();

		ConfrontTrespasser(MWTasks::Task* lifetask, MWWorld::Ptr mTarget);

		~ConfrontTrespasser();

		MWWorld::Ptr mTarget;

		virtual MWWorld::Ptr update();

		virtual int getTypeId() const;

		unsigned int mStep;

		MWWorld::Ptr mDestination;

		int mZoneSlotIdx;

		std::string mZoneId;

		std::string mDestId;

		bool mTalkedTo;

		bool mChasing;

	};

}








#endif