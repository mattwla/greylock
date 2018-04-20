#ifndef GAME_GLSUBBRAINS_SUBBRAIN_H
#define GAME_GLSUBBRAINS_SUBBRAIN_H
#include <components/esm/defs.hpp>
#include <string>
#include <vector>
#include <memory>
#include "../mwworld/ptr.hpp"
#include "../gllifemanager/goap.hpp"

namespace MWBase {
	class Life;
	class Awareness;
	struct GOAPStatus;
	struct GOAPData;
	class BehaviorObject;
	struct IntentionPlan;
};

namespace MWWorld {
	class Ptr;
}

namespace ESM {
	class RefNum;
}

namespace MWBase {

	typedef std::vector<GOAPStatus> statusstack;

	enum BOReturn {
		IN_PROGRESS = 0,
		COMPLETE = 1,
		FAILED = 2
	};

	//serves a a node for NPC planning
	struct GOAPData
	{
		statusstack mInputs;
		statusstack mOutputs;
		MWBase::BehaviorObject * mBehaviorObject;
		std::string mId; // for debugging.
		SmartEntityInstance * mSEI; //for the object the action will act upon. Like getting a BREAD or using an OVEN or talking to an NPC.
		int mCost = 0;
	};

	class BehaviorObject
	{
	protected:
		std::string mID;
		BehaviorObject * mSubBehavior; //maybe should instead be a GOAPDESIRE? or GOAPSTATUS?
		MWWorld::Ptr mOwnerPtr;
		ESM::RefNum mOwnerRefNum;
		MWBase::Life * mOwnerLife;
		std::shared_ptr<GOAPData> mGOAPData;
		SmartEntityInstance * mSEITarget;
		bool mInJourney;
		

	public:

		virtual BehaviorObject* Clone() = 0;

		virtual void getDebugInfo() = 0;

		void setOwner(MWBase::Life * life)
		{
			mOwnerLife = life;
		}

		virtual ~BehaviorObject() {

		};

		std::shared_ptr<GOAPData> getGOAPNode() {
			return mGOAPData;
		}

	
		virtual BOReturn start() = 0;

		virtual BOReturn update(float time, MWWorld::Ptr ownerptr) = 0;

		void setTarget(SmartEntityInstance * sei)
		{
			mSEITarget = sei;
		}

			//start
			//pause
			//run
	};
		
	struct GOAPStatus
	{
		enum StatusType {
			HAS_ID_IN_INVENTORY = 0,
			VITALS = 1,
			HAS_OBJECT_STATUS_IN_INVENTORY = 2,
			AWARE_OF_OBJECT_WITH_STATUS = 3
		};

		StatusType mStatusType;
		
		std::string mExtraData;
		
		int mAmount;

		bool operator==(GOAPStatus status);

		GOAPStatus::GOAPStatus()
		{

		};

		GOAPStatus::GOAPStatus(StatusType type, std::string extradata, int amount)
		{
			mStatusType = type;
			mExtraData = extradata;
			mAmount = amount;
		};

		
	};

	struct GOAPDesire
	{

		MWBase::GOAPStatus mStatus;
		int mValence;
		BehaviorObject * mIntentionPlan; //mwx fix me even needed?
		bool mIsIntention;
		GOAPDesire::GOAPDesire(MWBase::GOAPStatus stat, int val)
		{
			mStatus = stat;
			mValence = val;
			mIntentionPlan = 0;
			mIsIntention = false;
		};

		GOAPDesire::GOAPDesire()
		{

		};

	};
		

	class SubBrain 
	{

	protected:

		MWBase::Life * mOwnerLife;

		//Beliefs
		std::vector<GOAPStatus> mBeliefs;
		
		//Desire
		std::vector<GOAPDesire> mGOAPDesires;

		std::vector<std::shared_ptr<GOAPData>> mGOAPNodes;

		std::vector<WorldstateAtom> mWorldState;

	public:
		
		virtual ~SubBrain() {

		};
		
		virtual void calculate(MWBase::Awareness * awareness) = 0;

		virtual std::string getID() = 0;

		std::vector<GOAPDesire> getGOAPDesires()
		{
			return mGOAPDesires;
		};

		std::vector<WorldstateAtom> getWorldstate()
		{
			return mWorldState;
		}

		//Return BOs that have matching output, for now only BOs that totally meet needs
		virtual std::vector<std::shared_ptr<GOAPData>> getMatchingBehaviorObjects(MWBase::GOAPStatus);

		virtual void getDebugInfo() = 0;

	};

}


#endif