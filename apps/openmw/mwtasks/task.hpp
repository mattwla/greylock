#ifndef GAME_MWTASKS_TASK_H
#define GAME_MWTASKS_TASK_H
#include <components/esm/defs.hpp>

#include <string>

namespace MWWorld
{
	class Ptr;
}


namespace MWTasks
{

	class Task
	{
	public:
		Task();

		~Task();

		std::string mNpcId;

		bool mDone;

		void update();

		Task* mSubTask;


	};
}




#endif