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
#include "../mwbase/statusmanager.hpp"
#include "../mwstatus/statusmanagerimp.hpp"
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

		MWBase::StatusManager * mStatusManager = 0;

		std::vector<std::shared_ptr<MWBase::GOAPNodeData>> mGOAPNodeData;

		//====== FOR SEIS MANAGED BY SMARTZONES========

		bool mIsManagedBySmartzone = false;

		SmartEntityInstance * mMySmartZone;

		//====FOR SMART ZONES========

		bool mIsSmartZone = false;

		osg::BoundingBox mBoundingBox;

		std::vector<SmartEntityInstance*> mMyManagedEntities;
	
		std::vector<std::string> mAllowedNPCIds;

		//=====FOR LIFE=========

		bool mIsLife = false;

		MWBase::Life * mLife = 0;

		//========things not sure if needed================

		std::string mHomeCellName;

		bool mHomeCellIsExterior;

		int mHomeCellX;

		int mHomeCellY;

		MWWorld::LiveCellRefBase * mLiveCellRef;
		
		int mPingCount;

	public :
		
		virtual float getActivationDistance() {
			return 3000.0f;
			//default
		}

		virtual ~SmartEntityInstance() {

		};

		virtual MWBase::StatusManager * getStatusManager()
		{
			return mStatusManager;
		}


		void disable()
		{
			MWBase::Environment::get().getWorld()->disable(mPtr);
		}

		virtual void ensureStatusManager();
		
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

		virtual void startCharge(MWBase::Life * user)
		{
			std::cout << "template SEI charge logic" << std::endl;
			//generic charge logic;
		}

		virtual void onImpact(MWWorld::Ptr impactwith = nullptr);


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

		virtual bool isHumanLife()
		{
			return mIsLife;
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

		virtual void setLife(MWBase::Life * life)
		{
			mLife = life;
		}

		virtual MWBase::Life* getLife()
		{
			if (!mLife)
			{
				std::cout << "ERROR TRIED TO GET LIFE FROM NOT LIFE SEI" << std::endl;
				return nullptr;
			}
			else
			{
				return mLife;
			}

		}
		

	};

	class SmartEntityTemplate
	{
		
	protected: 
		//Should be vector in future, in runtime refids are checked against this to see which refs get smartentity data.
		std::vector<std::string> mIngameIDs;
	
	public:
		
		virtual ~SmartEntityTemplate() {

		};

		std::vector<std::string> getStringID();
		
		//will return a new SEI
		virtual SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr) = 0;
		
		//will reutrn a new SEI
		virtual SmartEntityInstance * getInstance(std::string id, ESM::RefNum refnum) = 0;
		
		//WIll create a new SEI and initialize it with data from serialized string
		virtual SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate) = 0;
	};

	class SmartEntitiesManager
	{
		//map of refid to smartentityconstructor
		typedef std::map<std::string, SmartEntityTemplate*> SmartTemplateMap; 
		
		//allows lookup of refid to see if it should have a smart template
		SmartTemplateMap mSmartTemplateMap;
		
		//allows lookup of refnum to SEI
		SmartInstanceMap mSmartInstanceMap;
		
		//list of all SEIs in active cells
		SmartInstanceMap mSmartInstancesInScene;
		
		//On new game, gets smartentity constructors loaded and builds smarttemplatemap
		void gatherSmartEntityTemplates();

		void addTemplateToMap(SmartEntityTemplate * set);

		//creates and inits an SEI with serialized data
		void loadSmartEntityInstance(std::string type, int contentnum, int index, std::string savestate);

		//for debugging
		int mRuntimeRefNumTicker;
	
	public:
		
		SmartEntitiesManager::SmartEntitiesManager();

		void onFrameUpdate(float duration);

		//Makes a smart instance from livecellref, or returns an already existing one
		MWBase::SmartEntityInstance * initializeInstFromLiveCellRef(MWWorld::LiveCellRefBase * livecellref);

		//returns an SEI, or makes a new one
		MWBase::SmartEntityInstance * getSmartEntityInstance(const MWWorld::Ptr &ptr, bool allowgeneric = false);

		//returns an SEI, or makes a new one
		//MWBase::SmartEntityInstance * getSmartEntityInstance(std::string id, ESM::RefNum refNum);

		//unused for now, will let an SEI remember the cell it originated from
		void registerHomeCell(const ESM::CellRef & cellref, const ESM::Cell * cell);

		//lets SEM know that a SEI has been added to scene
		void addSmartInstanceToScene(const MWWorld::Ptr &ptr);

		void removeSmartInstanceFromScene(const MWWorld::Ptr &ptr);

		//given a cell, SEM removes all instances that are in the cell from activeSEIS
		void removeSmartInstancesFromSceneViaCell(MWWorld::CellStore *cellStore);

		bool isInstanceInScene(const MWWorld::Ptr &ptr);

		bool hasSmartInstance(const MWWorld::Ptr &ptr);

		bool hasSmartInstance(ESM::RefNum refnum);

		bool hasSmartTemplate(MWWorld::LiveCellRefBase * lifecellref);

		SmartInstanceMap& getLiveSmartInstances();

		void outputInSceneInstancesToLog();

		void clear();

		void loadGame(boost::filesystem::path path);

		void saveGame(boost::filesystem::path path);

		//takes the active cell, inits all smartzones, inits all refids w/ templates, and makes sure all SEIS have updated pointers.
		void initializeActiveCell();

		//given an SEI and an SEIZone, lets each know about eachother
		bool linkSEtoZone(SmartEntityInstance * entity, SmartEntityInstance * zone);

		MWBase::SmartEntityInstance * refnumFetch(ESM::RefNum refnum);
		
		
	};


}
#endif
