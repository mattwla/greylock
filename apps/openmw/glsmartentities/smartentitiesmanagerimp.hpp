#ifndef GAME_MWMECHANICS_SMARTENTITIESMANAGER_H
#define GAME_MWMECHANICS_SMARTENTITIESMANAGER_H

#include "../mwbase/smartentitiesmanager.hpp"


#include "../mwworld/ptr.hpp"

#include <string>


namespace MWWorld
{
	class CellStore;
}

namespace GLSmartEntitiesManager
{


	class SmartEntitiesManager : public MWBase::SmartEntitiesManager
	{

		typedef std::map<std::string, MWBase::SmartEntityTemplate*> templateList;


		templateList mTemplateList;

	public:
		SmartEntitiesManager();

		virtual MWBase::SmartEntityInstance * getSmartEntityInstance(std::string id);
	};
}

#endif