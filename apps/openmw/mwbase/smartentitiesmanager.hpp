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
	
		std::string mRefId;
		
		MWWorld::Ptr mPtr;
		
		std::vector<std::string> mStatusList;

		std::vector<std::shared_ptr<MWBase::GOAPNodeData>> mGOAPNodeData;

		//====== FOR SEIS MANAGED BY SMARTZONES========

		bool mIsManagedBySmartzone = false;

		SmartEntityInstance * mMySmartZone;

		//====FOR SMART ZONES========

		bool mIsSmartZone = false;

		osg::BoundingBox mBoundingBox;

		std::vector<SmartEntityInstance*> mMyManagedEntities;
	
		std::vector<std::string> mAllowedNPCIds;

		//========things not sure if needed================

		std::string mHomeCellName;

		bool mHomeCellIsExterior;

		int mHomeCellX;

		int mHomeCellY;

		MWWorld::LiveCellRefBase * mLiveCellRef;
		
		int mPingCount;

	public :
		
		virtual float getActivationDistance() {
			return 1000.0f;
			//default
		}

		virtual ~SmartEntityInstance() {

		};
		
		//for debugging
		void ping();
		
		//for debugging
		void debugInfo();

		//returns true if SEI has a given string status, likely a temp solution 
		bool hasStatus(std::string status);
		
		//for debugging
		int getPings();
		
		//amount of lifes currently using SEI
		int mCurrentUserCount = 0;
		
		//returns the string refid of sei
		std::string getRefId();

		//returns the refnum of SEI
		ESM::RefNum getRefNum();
		
		//returns the ptr of SEI
		MWWorld::Ptr & getPtr();
		
		//let SEI know if its new ptr
		void updatePtr(MWWorld::Ptr ptr);
		
		//the cellstore of the SEIS original reference
		void registerHomeCell(const ESM::Cell * cell);

		//get serial data
		virtual std::string getSaveString() = 0;

		//Currently for when NPC wants to use an object in inventory. Maybe this is more like consume? Probably a temp implementation
		virtual bool use(MWBase::Life * user) {

			std::cout << "invalid use " << std::endl;
			return false;
		}

		//Can it be used?
		virtual bool isAvailableForUse()
		{
			//default behavior, true
			return true;
		}

		//For using an SEI that lives in the world.
		virtual MWBase::BehaviorObject * useWorldInstance(MWBase::Life * user)
		{
			std::cout << "not able to be used in world" << std::endl;
			return NULL;
		}

		//Change so works much more like subbrain. 
		virtual std::vector<std::shared_ptr<MWBase::GOAPNodeData>> getGOAPNodeData()
		{
			return mGOAPNodeData;
		};

		//Get bounding box of item, used for smartzones
		virtual void buildBoundingBox()
		{
			std::cout << "Something that is not a smartzone had a request to build a bounding box" << std::endl;
		}

		virtual bool isSmartZone()
		{
			return mIsSmartZone;
		}

		//Is the given ptr in this smartzones bounding box?
		virtual bool containsPtr(MWWorld::Ptr ptr)
		{
			std::cout << "warning: asked not a smartzone if it contained something" << std::endl;
			return false;
		}

		//Temporary and hacky, adds a ptr to the list of npcs allowed to use items in the smartzone. Lame for now.
		virtual void addAllowedNPC(MWWorld::Ptr ptr)
		{
			if (!mIsSmartZone)
				std::cout << "ERROR tried to add permission to something that is not a smart zone" << std::endl;
			else
				mAllowedNPCIds.push_back(ptr.getCellRef().getRefId());
		}

		//Temporary and hacky, lets an SEI know which smartzone it belongs to and flags it as managed by a smartzone
		virtual void assignZone(SmartEntityInstance * zone)
		{
			mMySmartZone = zone;
			mIsManagedBySmartzone = true;
		}

		//temp and hacky, adds an instance to the smartzone for management
		virtual void giveEntityToManage(SmartEntityInstance * instance)
		{

			mMyManagedEntities.push_back(instance);
		}

		virtual bool isManagedBySmartZone()
		{
			return mIsManagedBySmartzone;
		}

		virtual SmartEntityInstance * getSmartZone()
		{
			return mMySmartZone;
		}

		//hacky and temporary, returns true if given life is allowed to use smart zone.
		virtual bool isAllowedTerritory(MWBase::Life * life);

			

		virtual MWBase::SubBrain * getSubBrain(MWBase::Life * life)
		{
			std::cout << "ERROR SUBBRAIN REQUESTED FROM SEI WHICH DOES NOT HAVE ONE TO GIVE" << std::endl;
			return nullptr;
		}
		

	};

	class SmartEntityTemplate
	{
		
	protected: 
		//Should be vector in future, in runtime refids are checked against this to see which refs get smartentity data.
		std::string mIngameID;
	
	public:
		
		virtual ~SmartEntityTemplate() {

		};

		std::string getStringID();
		
		//will return an existing SEI, or create a new one
		virtual SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr) = 0;
		
		//will reutrn an existing SEI, or create a new one
		virtual SmartEntityInstance * getInstance(std::string id, ESM::RefNum refnum) = 0;
		
		//WIll create a new SEI and initialize it with data from serialized string
		virtual SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate) = 0;
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

		bool linkSEtoZone(SmartEntityInstance * entity, SmartEntityInstance * zone);
		
		
	};


}
#endif
