#ifndef GAME_MWMECHANICS_LIFEMANAGER_H
#define GAME_MWMECHANICS_LIFEMANAGER_H

#include "../mwbase/lifemanager.hpp"

#include "../mwworld/ptr.hpp"

#include <string>


namespace MWWorld
{
	class CellStore;
}

namespace GLLifeManager
{
	class LifeManager : public MWBase::LifeManager
	{

	public:
		LifeManager();

		virtual void update(float duration, bool paused);

		virtual void initialize();


	};
}

#endif