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

	struct GOAPData
	{
		statusstack mInputs;
		statusstack mOutputs;
		MWBase::BehaviorObject * mBehaviorObject;
		std::string mId; // for debugging.
		SmartEntityInstance * mSEI;
		int mCost = 0;


		//target SmartEntity (what are we acting on?)
		//check preconditions.... is this the "expanding node" funcitonality I am thinking about? Does it call the subbrain? 
		//Maybe BOs don't NEED to be connected to subbrains.
		//getcost
		//check inputs valid (pass ref to statuslist)

		//Current input valid?
		//==?
		//int getCost(bigfive, awareness) return 0 if impossible
		//getLinkedBO
		//bool outputTypeMatch(type)



			
	};

	class BehaviorObject
	{

	protected:
		
		bool mIsDesire;
		int mValence;
		std::string mID;
		BehaviorObject * mSubBehavior;
		MWWorld::Ptr mOwnerPtr;
		ESM::RefNum mOwnerRefNum;
		std::shared_ptr<GOAPData> mGOAPData;

	public:
	/*	bool isDesire()
		{
			return mIsDesire;
		};*/

		virtual BehaviorObject* Clone() = 0;

		virtual void getDebugInfo() = 0;

		virtual ~BehaviorObject() {

		};

		std::shared_ptr<GOAPData> getGOAPNode() {
			return mGOAPData;
		}

		void setValence(int valence)
		{
			mValence = valence;
		}

		virtual BOReturn update(float time) = 0;

			

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
		BehaviorObject * mIntentionPlan;
		//MWBase::IntentionPlan mRealIntentionPlan;
		bool mIsIntention;

		GOAPDesire::GOAPDesire(MWBase::GOAPStatus stat, int val)
		{
			mStatus = stat;
			mValence = val;
			mIntentionPlan = 0;
			mIsIntention = false;
		}

	};
		




	class SubBrain 
	{

	protected:

		MWBase::Life * mOwnerLife;

		std::vector<GOAPStatus> mBeliefs;
		//Beliefs
		
		std::vector<BehaviorObject*> mDesires;
		//Desires

		std::vector<GOAPDesire> mGOAPDesires;

		std::vector<std::shared_ptr<GOAPData>> mGOAPNodes;

	

		std::vector<WorldstateAtom> mWorldState;

		
		//Intentions

	public:
		
		virtual ~SubBrain() {

		};
		
		virtual void calculate(MWBase::Awareness * awareness) = 0;

		virtual std::string getID() = 0;

		std::vector<BehaviorObject*> getDesires()
		{
			return mDesires;
		};

		std::vector<GOAPDesire> getGOAPDesires()
		{
			return mGOAPDesires;
		};

		std::vector<std::shared_ptr<GOAPData>> getGOAPNodes()
		{
			return mGOAPNodes;
		}

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