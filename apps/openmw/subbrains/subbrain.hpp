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
	struct GOAPNodeData;
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
	struct GOAPNodeData
	{
		statusstack mInputs; //AND or OR. Logic inside this to determine that.
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
		bool mUsingSubBehavior = false;
		MWWorld::Ptr mOwnerPtr;
		ESM::RefNum mOwnerRefNum;
		MWBase::Life * mOwnerLife;
		std::shared_ptr<GOAPNodeData> mGOAPNodeData;
		SmartEntityInstance * mSEITarget;
		bool mInJourney;
		bool mStopRequested = false;
		

	public:

		virtual BehaviorObject* Clone() = 0;

		virtual void getDebugInfo() = 0;

		void setOwner(MWBase::Life * life)
		{
			mOwnerLife = life;
		}

		virtual ~BehaviorObject() {

		};

		std::shared_ptr<GOAPNodeData> getGOAPNode() {
			return mGOAPNodeData;
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

		virtual bool stop()
		{
			//by default no stop logic;
			std::cout << "BO with no stop logic recieved stop request" << std::endl;
			return false;
		};
	};
		
	struct GOAPStatus
	{
		enum StatusType {
			HAS_ID_IN_INVENTORY = 0,
			VITALS = 1,
			HAS_OBJECT_STATUS_IN_INVENTORY = 2,
			AWARE_OF_OBJECT_WITH_STATUS = 3,
			STATUS_VOID = 4
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
		std::string debugInfo = "desire template";
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

		bool operator> (const GOAPDesire& gd2) {
			return mValence > gd2.mValence;
		}

	};
		

	class SubBrain 
	{

	protected:

		MWBase::Life * mOwnerLife;

		//Beliefs
		std::vector<GOAPStatus> mBeliefs;
		
		//Desire
		//std::vector<GOAPDesire> mGOAPDesires;

		std::vector<std::shared_ptr<GOAPNodeData>> mGOAPNodes;

		std::vector<WorldstateAtom> mWorldState;

	public:
		
		virtual ~SubBrain() {

		};
		
		virtual void calculate(MWBase::Awareness * awareness) = 0;

		virtual std::string getID() = 0;

		//virtual std::vector<std::shared_ptr<GOAPDesire>> getGOAPDesires() = 0;
	
		std::vector<WorldstateAtom> getWorldstate()
		{
			return mWorldState;
		}

		//Return BOs that have matching output, for now only BOs that totally meet needs
		virtual std::vector<std::shared_ptr<GOAPNodeData>> getMatchingBehaviorObjects(MWBase::GOAPStatus);

		virtual void getDebugInfo() = 0;

	};

}


#endif