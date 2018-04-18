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

#include "../mwworld/ptr.hpp"
#include "../mwtasks/task.hpp"
#include "../mwbase/statusmanager.hpp"
#include "../mwbase/aischedulemanager.hpp"
#include "../mwtasks/life.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../subbrains/subbrain.hpp"
#include "../gllifemanager/goap.hpp"



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




namespace MWBase
{

	struct BehaviorIntentionQueue 
	{
		std::vector<BehaviorObject> mQueue;
		int mCurrentFocus;
		int mLastFocus;
	};

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

		std::string getSaveState();
		
		void loadState(std::string);
	};

	struct IntentionPlan
	{
		MWBase::GOAPStatus mDesiredState;
		std::vector<std::shared_ptr<GOAPData>> mGOAPDataList;
		int mCurrentStep;
		bool mPlanComplete;
	};

	class SubBrainsManager
	{
		std::vector<SubBrain*> mSubBrains;

		//Beliefs, states from the subbrains, combined with sensorystore. Probe sensory store w/ status?

		std::vector<BehaviorObject*> mDesires;
		//Desires, recommended BOs from the subbrains

		std::vector<GOAPDesire> mGOAPDesires;

		std::vector<std::shared_ptr<GOAPData>> mGOAPNodes;

		std::vector<WorldstateAtom> mWorldState;

		MWBase::Life * mLife;

		//void seperateCompletePlans(std::vector<IntentionPlan> & planlist, std::vector<IntentionPlan> & completelist);

		//Intention... selected by the a higher order, life manager. Determined by selected desire + any GOAP plans the BO requests.

	public:

		void seperateCompletePlans(std::vector<IntentionPlan>& planlist, std::vector<IntentionPlan>& completelist, MWWorld::Ptr ptr);

		//takes in a goap status and an npc, returns true if status is met and false otherwise
		bool evaluateGOAPStatus(MWBase::GOAPStatus status, MWWorld::Ptr ptr);

		IntentionPlan createIntention(MWBase::GOAPStatus status, MWWorld::Ptr ptr);

		std::vector<std::shared_ptr<GOAPData>> querySubBrainsForGOAPMatches(MWBase::GOAPStatus status);

		bool hasObjectStatusInInventory(MWBase::GOAPStatus status, MWWorld::Ptr ptr);

		bool hadObjectStatusInAwareness(std::string status, MWWorld::Ptr ptr);

		void calculate(MWBase::Awareness * awareness);

		SubBrainsManager::SubBrainsManager(MWBase::Life * life);

		std::vector<std::string> getSaveStates();

		void logDesires();

		void logWorldstate();

		std::vector<BehaviorObject*> getDesires();

		std::vector<GOAPDesire> getGOAPDesires();

		std::vector<WorldstateAtom> getWorldstate();

		std::vector<std::shared_ptr<GOAPData>> getGOAPNodes()
		{
			return mGOAPNodes;
		}

		void loadStates();

		//SubBrainsManager::SubBrainsManager();

	};
	
	
	struct Life
	{
		std::string mId;

		int mRefNum;

		MWWorld::CellStore *mOwnerCell;

		MWBase::Awareness *mAwareness;

		MWBase::SubBrainsManager *mSubBrainsManager;

		MWWorld::Ptr mPtr;

		BigFive mBigFive;

		Vitals mVitals;

		IntentionPlan mCurrentIntentionPlan;

		BehaviorIntentionQueue *mBIQueue;

		bool mHasIntention = false;




		//IntentionList(Active BOs)

	public:

		Life(std::string id)
		{
			mPtr = MWBase::Environment::get().getWorld()->searchPtr(id, false);
			mId = id;
			mRefNum = mPtr.getCellRef().getRefNum().mIndex;
			mOwnerCell = mPtr.getCell();
			mPtr.getBase()->mLife = this;
			mAwareness = new MWBase::Awareness(mPtr);
			mSubBrainsManager = new MWBase::SubBrainsManager(this);
			mBIQueue = new BehaviorIntentionQueue();
			
		}

		void getDebugInfo();

		void update(float duration);

	private:

		void metabolize(float duration);

		std::vector<BehaviorObject*> prioritizeDesires(std::vector<BehaviorObject*> desires); 

		void prioritizeDesires(std::vector<GOAPDesire> &desires);


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
