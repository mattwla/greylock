#ifndef GAME_MWBASE_SMARTENTITIES_H
#define GAME_MWBASE_SMARTENTITIES_H

#include <string>
#include <vector>
#include <list>
#include <set>
#include <stdint.h>
#include <map>

#include <list>

#include <components/esm/loadpgrd.hpp>
#include "../mwmechanics/pathgrid.hpp"
#include "../mwworld/timestamp.hpp"

#include "../mwworld/ptr.hpp"
#include "../mwtasks/task.hpp"


#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>



namespace osg
{
	class Vec3f;
}

namespace ESM
{
	struct Class;

	class ESMReader;
	class ESMWriter;
}

namespace MWWorld
{
	class Ptr;
	class CellStore;
	class CellRef;
}

namespace Loading
{
	class Listener;
}



namespace MWBase
{

	class SmartEntityInstance
	{
		
	protected:
		int mPingCount;

	public :
		void ping();
		int getPings();

	};

	
	
	
	class SmartEntityTemplate
	{
		
	protected: 
		std::string mIngameID;

	public:
		std::string getStringID();

		virtual SmartEntityInstance * getInstance() = 0;

	};



	
	
	class SmartEntitiesManager
	{

		typedef std::map<std::string, SmartEntityTemplate*> SmartTemplateMap; //allows an item to lookup if it should have smartentity functions by it's own ID
		typedef std::map<int, SmartEntityInstance*> SmartInstanceMap;
		SmartTemplateMap mSmartTemplateMap;
		SmartInstanceMap mSmartInstanceMap;
		
		void gatherSmartEntityTemplates();

	

	public:
		
		SmartEntitiesManager::SmartEntitiesManager();

		void clear();

		void newGame();

		void loadGame(std::string savename);

		void saveGame();

		SmartEntityInstance * getSmartEntityInstance(const MWWorld::Ptr &ptr);

		bool hasSmartInstance(const MWWorld::Ptr &ptr)
		{
			int refnum = ptr.getBase()->mRef.getRefNum().mIndex;
			if (!mSmartInstanceMap.count(refnum))
				return false;
			else
				return true;
		}


	};

}
#endif
