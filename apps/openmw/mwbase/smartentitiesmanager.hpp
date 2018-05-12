#ifndef GAME_MWBASE_SMARTENTITIES_H
#define GAME_MWBASE_SMARTENTITIES_H

#include <string>
#include <vector>
#include <list>
#include <set>
#include <stdint.h>
#include <map>
#include <list>
#include "../mwbase/lifemanager.hpp"
#include "../subbrains/subbrain.hpp"
#include <components/esm/loadpgrd.hpp>
#include "../mwmechanics/pathgrid.hpp"
#include "../mwworld/timestamp.hpp"
#include "../mwworld/ptr.hpp"
#include "../mwtasks/task.hpp"
#include <osg/BoundingBox>

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>
#include <boost/filesystem/path.hpp>
#include <vector>

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
	typedef std::map<ESM::RefNum, SmartEntityInstance*> SmartInstanceMap; //should be unordered?
	
	
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

		std::vector<std::string> mStatusList;

		//std::vector<MWBase::GOAPStatus> mGOAPList;

		std::vector<std::shared_ptr<MWBase::GOAPNodeData>> mGOAPNodeData;

		osg::BoundingBox mBoundingBox;

		bool mIsSmartZone = false;
		

		//Get available BehaviorObjects

		

	public :
		


		virtual float getActivationDistance() {
			return 1000.0f;
			//default
		}

		virtual ~SmartEntityInstance() {

		};
		
		void ping();
		
		void debugInfo();

		bool hasStatus(std::string status);
		
		int getPings();
		
		int mCurrentUserCount = 0;
		
		std::string getRefId();

		ESM::RefNum getRefNum();
		
		MWWorld::Ptr & getPtr();
		
		void updatePtr(MWWorld::Ptr ptr);
		
		void registerHomeCell(const ESM::Cell * cell);

		virtual std::string getSaveString() = 0;

		virtual bool use(MWBase::Life * user) {

			std::cout << "invalid use " << std::endl;
			return false;
		}

		virtual bool isAvailableForUse()
		{
			//default behavior, true
			return true;
		}

		virtual MWBase::BehaviorObject * useWorldInstance(MWBase::Life * user)
		{
			std::cout << "not able to be used in world" << std::endl;
			return NULL;
		}

		virtual bool meetsInputNeed(MWBase::GOAPStatus status)
		{
			return true;
		}

		virtual std::vector<std::shared_ptr<MWBase::GOAPNodeData>> getGOAPNodeData()
		{
			return mGOAPNodeData;
		};

		virtual void buildBoundingBox()
		{
			std::cout << "Something that is not a smartzone had a request to build a bounding box" << std::endl;
		}

		virtual bool isSmartZone()
		{
			return mIsSmartZone;
		}

		


	};

	class SmartEntityTemplate
	{
		
	protected: 
		

		//all possible BehaviorObjects (some can be enabled or disabled depending)

		std::string mIngameID;
	
	public:
		
		virtual ~SmartEntityTemplate() {

		};

		std::string getStringID();
		
		virtual SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr) = 0;
		
		virtual SmartEntityInstance * getInstance(std::string id, ESM::RefNum refnum) = 0;
		
		virtual SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate) = 0;
	};


	class SmartZoneTemplate : public SmartEntityTemplate
	{

		std::vector<SmartEntityInstance> mOwnedSEIList;

	};


	class SmartEntitiesManager
	{
		typedef std::map<std::string, SmartEntityTemplate*> SmartTemplateMap; //allows an item to lookup if it should have smartentity functions by it's own ID
		
		SmartTemplateMap mSmartTemplateMap;
		
		SmartInstanceMap mSmartInstanceMap;
		
		SmartInstanceMap mSmartInstancesInScene;
		
		void gatherSmartEntityTemplates();

		void loadSmartEntityInstance(std::string type, int contentnum, int index, std::string savestate);

		//void loadSmartEntityInstance(std::string type, int contentnum, int index, int pings);
		
		int mRuntimeRefNumTicker;
	
	public:
		
		SmartEntitiesManager::SmartEntitiesManager();

		MWBase::SmartEntityInstance * initializeInstFromLiveCellRef(MWWorld::LiveCellRefBase * livecellref);

		MWBase::SmartEntityInstance * getSmartEntityInstance(const MWWorld::Ptr &ptr);

		MWBase::SmartEntityInstance * getSmartEntityInstance(std::string id, ESM::RefNum refNum);

		void registerHomeCell(const ESM::CellRef & cellref, const ESM::Cell * cell);

		void addSmartInstanceToScene(const MWWorld::Ptr &ptr);

		void removeSmartInstanceFromScene(const MWWorld::Ptr &ptr);

		void removeSmartInstancesFromSceneViaCell(MWWorld::CellStore *cellStore);

		bool isInstanceInScene(const MWWorld::Ptr &ptr);

		bool hasSmartInstance(const MWWorld::Ptr &ptr);

		bool hasSmartInstance(ESM::RefNum refnum);

		bool hasSmartTemplate(std::string id);

		SmartInstanceMap& getLiveSmartInstances();

		void outputInSceneInstancesToLog();

		void clear();

		void loadGame(boost::filesystem::path path);

		void saveGame(boost::filesystem::path path);

		void initializeActiveCell();
		
	};


}
#endif
