#ifndef GAME_MWBASE_LIFEMANAGER_H
#define GAME_MWBASE_LIFEMANAGER_H

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
#include "../mwbase/tasksmanager.hpp"
#include "../mwworld/ptr.hpp"
#include "../mwtasks/task.hpp"
#include "../mwbase/statusmanager.hpp"
#include "../mwbase/aischedulemanager.hpp"
#include "../mwtasks/life.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../subbrains/subbrain.hpp"
#include "../gllifemanager/goap.hpp"
#include "../mwtasks/task.hpp"
#include "../mwtasks/journey.hpp"



#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>
#include <boost/filesystem/fstream.hpp>

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

//namespace MWTasks
//{
//	class JourneyManager;
//}

namespace MWBase
{
	struct BigFive 
	{
		int mOpenness;
		int mConscientiousness;
		int mExtraversion;
		int mAgreeableness;
		int mNeuroticism;
	};

	struct Vitals 
	{
		float mHunger = 0.0f;
		float mSleepiness = 0.0f;
		float mStress = 0.0f;
		std::string getSaveState();
		void loadState(std::string);
	};

	struct IntentionPlan
	{
		MWBase::GOAPStatus mDesiredState;
		std::vector<std::shared_ptr<GOAPNodeData>> mGOAPNodeDataList;
		int mCurrentStep;
		bool mPlanComplete;
		BehaviorObject * mCurrentBehaviorObject = 0;
		std::shared_ptr<MWBase::GOAPDesire> mDesire;
		bool stop();

		IntentionPlan(std::shared_ptr<MWBase::GOAPDesire> desire) : mDesire(desire)
		{
			
		}

		IntentionPlan()
		{

		}
	};

	enum LifeFSMState
	{
		FSM_THINKING = 0,
		FSM_DOING = 1,
		FSM_TRAVELLING = 2
	};

	class SubBrainsManager
	{
		std::vector<SubBrain*> mSubBrains; //process objects in world, create goals to accomplish (desires), offer ways to solve problems (intentions)
		std::vector<GOAPDesire> mGOAPDesires;
		std::vector<std::shared_ptr<GOAPNodeData>> mGOAPNodes;
		MWBase::Life * mLife;
		//std::vector<WorldstateAtom> mWorldState;
	
	
	public:

		

		SubBrainsManager::SubBrainsManager(MWBase::Life * life);

		//parses current plans to accomplish a given intention, seperates those that are ready to do vs those that need more planning
		void seperateCompletePlans(std::vector<IntentionPlan>& planlist, std::vector<IntentionPlan>& completelist, MWWorld::Ptr ptr);

		//takes in a goap status and an npc, returns true if input status is met and false otherwise
		bool evaluateGOAPStatus(MWBase::GOAPStatus status, MWWorld::Ptr ptr);

		//takes in an npc ptr and a status, returns true if the npc has an object w/ that status in their inventory
		bool hasObjectStatusInInventory(MWBase::GOAPStatus status, MWWorld::Ptr ptr);
		
		//takes in an npc ptr and a status, returns true if npc has an object w/ that status in their awareness stores (they know of one that exists)
		bool hadObjectStatusInAwareness(std::string status, MWWorld::Ptr ptr);

		//Given a desired status to achieve, creates a chain of behavior objects which can accomplish the task if run in sequence
		IntentionPlan createIntention(MWBase::GOAPStatus status, MWWorld::Ptr ptr);

		//Given a status, queries all the subrains for behavior objectas that can help make that status true
		std::vector<std::shared_ptr<GOAPNodeData>> querySubBrainsForGOAPMatches(MWBase::GOAPStatus status);

		//run every frame, lets subbrains calculate ways to react to world
		void calculate(MWBase::Awareness * awareness);

		//serializes all info in the sub brain manager, used for recording a savestate
		std::vector<std::string> getSaveStates();

		//debugging
		void logDesires();

		//debugging
		void logWorldstate();

		//std::vector<BehaviorObject*> getDesires();

		std::vector<GOAPDesire> getGOAPDesires();

		//std::vector<WorldstateAtom> getWorldstate();

		std::vector<std::shared_ptr<GOAPNodeData>> getGOAPNodes()
		{
			return mGOAPNodes;
		}

		void loadStates();

		//SubBrainsManager::SubBrainsManager();

	};
	
	
	struct Life
	{
		//id of npc who life belongs to, good for looking up outdated ptr
		std::string mId;

		//int mRefNum;

		ESM::RefNum mRefNum;

		//Not used now, perhaps potential solution for outdated ptrs in future
		//MWWorld::CellStore *mOwnerCell;

		//Awareness manager of npc, used to figure out what npc is aware of, and store things npc has seen in their memory
		MWBase::Awareness *mAwareness;

		//Calculated NPC reactions to world, hunger, desires, goals, the NPCs behavior essentially
		MWBase::SubBrainsManager *mSubBrainsManager;

		//ptr to npc
		MWWorld::Ptr mPtr;

		//NPCs personality
		BigFive mBigFive;

		//NPCs vitals, health, hunger, sleepiness, etc.
		Vitals mVitals;

		//Current plan NPC is working towards
		//IntentionPlan mCurrentIntentionPlan;

		//try to keep this at 2 at max.
		IntentionPlan mCurrentIntentionPlan;

		IntentionPlan mQueuedIntentionPlan;

		//Does the NPC have an intention now? Should be FSM of THINKING, IN_ACTION, OR TRAVELLING
		bool mHasIntention = false;

		bool mHasQueuedIntention = false;

		LifeFSMState mFSMState;

		MWBase::JourneyManager * mJourneyManager;

		MWBase::SmartEntityInstance * getSEIWithStatusFromInventory(std::string status);

		


	public:

		Life(std::string id)
		{
			mPtr = MWBase::Environment::get().getWorld()->searchPtr(id, false);
			mId = id;
			mRefNum = mPtr.getCellRef().getRefNum();
			//mOwnerCell = mPtr.getCell();
			mPtr.getBase()->mLife = this;
			mAwareness = new MWBase::Awareness(mPtr);
			mSubBrainsManager = new MWBase::SubBrainsManager(this);
			mJourneyManager = new MWBase::JourneyManager(this);
			mFSMState = FSM_THINKING;
		}

		std::vector<std::shared_ptr<MWBase::GOAPDesire>> mDesireList;

		void getDebugInfo();

		void update(float duration);

		void submitDesirePtr(std::shared_ptr<MWBase::GOAPDesire> desire);

	private:

		void metabolize(float duration);

		//std::vector<BehaviorObject*> prioritizeDesires(std::vector<BehaviorObject*> desires); 

		void prioritizeDesires();

		void determineIntention();

		IntentionPlan selectIntentionPlan(std::shared_ptr<GOAPDesire> desire);

		void runTopIntentionPlan(float duration);

		void runSwapIntentionPlan(float duration);

		bool mSuccsessfulStopRequest;
		
	
	};

	class LifeManager
	{
		
	public:

		virtual void newGame() = 0;
		
		virtual void update(float duration, bool paused) = 0;

		virtual void initialize() = 0;

		virtual ~LifeManager() {}

		std::vector<MWBase::Life*> mLifeList;

		virtual bool inActiveRange(MWWorld::Ptr npc) = 0;

		float mLastTimeReported;

		float mTimePassed;

		float mTimeAccumulator;

		void loadGame(boost::filesystem::path path);

		void saveGame(boost::filesystem::path path);

	};

	
}
#endif
