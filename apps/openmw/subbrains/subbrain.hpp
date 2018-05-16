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
	
	enum StatusType {
		HAS_ID_IN_INVENTORY = 0,
		VITALS = 1,
		HAS_OBJECT_STATUS_IN_INVENTORY = 2,
		AWARE_OF_OBJECT_WITH_STATUS = 3,
		STATUS_VOID = 4
	};

	struct GOAPStatus
	{
	
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
		ESM::RefNum mRefNum;
		//SmartEntityInstance * mSEI; //for the object the action will act upon. Like getting a BREAD or using an OVEN or talking to an NPC.
		int mCost = 0;
		MWBase::BehaviorObject * getNewBehaviorObject(MWBase::Life * ownerlife, ESM::RefNum seirefnum);

		//constructor for objects with single input and output
		GOAPNodeData(MWBase::GOAPStatus input, MWBase::GOAPStatus output, MWBase::BehaviorObject * behaviorobject, ESM::RefNum refnum, int cost, std::string debugid)
		{
			statusstack inputs;
			inputs.push_back(input);
			mInputs = inputs;

			statusstack outputs;
			outputs.push_back(output);
			mOutputs = outputs;

			mBehaviorObject = behaviorobject;

			mRefNum = refnum;

			mCost = cost;

			mId = debugid;

		}

		//constructor for objects with multiple inputs and outputs
		GOAPNodeData(statusstack input, statusstack output, MWBase::BehaviorObject * behaviorobject, ESM::RefNum refnum, int cost, std::string debugid) {

			mInputs = input;

			mOutputs = output;

			mBehaviorObject = behaviorobject;

			mRefNum = refnum;

			mCost = cost;

			mId = debugid;

		};
	};

	class BehaviorObject
	{
	protected:
		std::string mID;
		BehaviorObject * mSubBehavior; //maybe should instead be a GOAPDESIRE? or GOAPSTATUS?
		bool mUsingSubBehavior = false;
		MWWorld::Ptr mOwnerPtr;
		ESM::RefNum mOwnerRefNum;
		ESM::RefNum mTargetRefNum;
		MWBase::Life * mOwnerLife;
		//std::shared_ptr<GOAPNodeData> mGOAPNodeData;
		SmartEntityInstance * mSEITarget;
		bool mInJourney = false;
		bool mStopRequested = false;
		//MWBase::SensoryLinkStore * mOwnerSensoryStore;
		

	public:
		 
		virtual BehaviorObject* Clone(MWBase::Life * life, ESM::RefNum refnum) = 0;

		virtual void getDebugInfo() = 0;

		void setOwner(MWBase::Life * life)
		{
			mOwnerLife = life;
		}

		virtual ~BehaviorObject() {

		};

		//std::shared_ptr<GOAPNodeData> getGOAPNode() {
		//	//return mGOAPNodeData;
		//}

	
		virtual BOReturn start() = 0;

		virtual BOReturn update(float time, MWWorld::Ptr ownerptr) = 0;

		void setTarget(SmartEntityInstance * sei)
		{
			mSEITarget = sei;
		}

			//start
			//pause
			//run

		virtual bool stop() = 0;
		
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
		

	class MetaBrain //manages subbrains for group behavior or behavior determined by logic embedded in environment
	{

	};

	class SubBrain //used by NPCs to make decisions and solve problems
	{

	protected:

		MWBase::Life * mOwnerLife;

		//Beliefs
		std::vector<GOAPStatus> mBeliefs;
		
		//Desire
		//std::vector<GOAPDesire> mGOAPDesires;

		//std::vector<std::shared_ptr<GOAPNodeData>> mGOAPNodes;

		std::vector<WorldstateAtom> mWorldState;

		MetaBrain * mMetaBrain;

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