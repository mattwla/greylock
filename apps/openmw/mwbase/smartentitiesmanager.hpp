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
#include <boost/filesystem/path.hpp>

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
	typedef std::map<int, SmartEntityInstance*> SmartInstanceMap;

	class SmartEntityInstance
	{
	protected:
		int mPingCount;
		std::string mRefId;
		MWWorld::Ptr mPtr; //will be problem.
		MWWorld::LiveCellRefBase * mLiveCellRef;
		std::string mHomeCellName;
		bool mHomeCellIsExterior;
		int mHomeCellX;
		int mHomeCellY;

	public :
		void ping();
		void debugInfo();
		int getPings();
		std::string getRefId();
		MWWorld::Ptr & getPtr();
		void updatePtr(MWWorld::Ptr ptr);
		//void updatePtr(MWWorld::Ptr & ptr);
		void registerHomeCell(const ESM::Cell * cell);
	};

	class SmartEntityTemplate
	{
		
	protected: 
		std::string mIngameID;
	public:
		std::string getStringID();
		virtual SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr) = 0;
		virtual SmartEntityInstance * getInstance(std::string id, int refnum) = 0;
		virtual SmartEntityInstance * loadInstance(std::string refid, int refnum, int pings) = 0;
	};

	class SmartEntitiesManager
	{
		typedef std::map<std::string, SmartEntityTemplate*> SmartTemplateMap; //allows an item to lookup if it should have smartentity functions by it's own ID
		SmartTemplateMap mSmartTemplateMap;
		SmartInstanceMap mSmartInstanceMap;
		SmartInstanceMap mSmartInstancesInScene;
		void gatherSmartEntityTemplates();
		void loadSmartEntityInstance(std::string type, int refnum, int pings);
		int mRuntimeRefNumTicker;
	public:
		
		SmartEntitiesManager::SmartEntitiesManager();

		void clear();

		void loadGame(boost::filesystem::path path);

		void saveGame(boost::filesystem::path path);

		SmartEntityInstance * getSmartEntityInstance(const MWWorld::Ptr &ptr);

		MWBase::SmartEntityInstance * getSmartEntityInstance(std::string id, int refNum);

		void registerHomeCell(const ESM::CellRef & cellref, const ESM::Cell * cell);

		//void registerHomeCell(const MWWorld::CellRef * cellref, std::string name, bool isExterior, int x, int y);

		MWBase::SmartEntityInstance * initializeInstFromLiveCellRef(MWWorld::LiveCellRefBase * livecellref);

		void addSmartInstanceToScene(const MWWorld::Ptr &ptr);

		bool isInstanceInScene(const MWWorld::Ptr &ptr);

		void removeSmartInstanceFromScene(const MWWorld::Ptr &ptr);

		void removeSmartInstancesFromSceneViaCell(MWWorld::CellStore *cellStore);

		bool hasSmartInstance(const MWWorld::Ptr &ptr);

		bool hasSmartInstance(int refnum);

		SmartInstanceMap& getLiveSmartInstances();

		void outputInSceneInstancesToLog();
		
	};

}
#endif
