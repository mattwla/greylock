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
	class SubBrain;
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
		STATUS_VOID = 4,
		REMOVE_PERSON_FROM_ZONE = 5,
		RUNNING_BEHAVIOR_OBJECT = 6
		
	};

	struct GOAPStatus
	{
		//Broad category of status.
		StatusType mStatusType;
		//Specific data relative to status type
		std::string mExtraData;
		//amount of status if applicable
		int mAmount;

		//Inflict status on who/what?
		MWBase::SmartEntityInstance * mTarget;
		//Allows checking if two status are functionally equivalent. 
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

		GOAPStatus::GOAPStatus(StatusType type, std::string extradata, int amount, MWBase::SmartEntityInstance * target)
		{
			mStatusType = type;
			mExtraData = extradata;
			mAmount = amount;
			mTarget = target;
		}
	};

	//Many behavior objects will need a few statuses to be true, or will output a few different effects. So we will be working with GOAPStatus vectors a lot.
	typedef std::vector<GOAPStatus> statusstack;

	//When a BehaviorObject updates, returns one of these.
	enum BOReturn {
		IN_PROGRESS = 0,
		COMPLETE = 1,
		FAILED = 2,
		STOPPED = 3,
		PAUSED = 4
	};

	//serves a a node for NPC planning.
	struct GOAPNodeData
	{
		//statuses that need to be true for this BO to work
		statusstack mInputs;
		
		//Results of completing this Bo
		statusstack mOutputs;
		
		//If we end up deciding to use a plan that utlized this node, this is a link to create a new BehaviorObject from it
		MWBase::BehaviorObject * mBehaviorObjectTemplate;
		
		// for debugging.
		std::string mId; 
		
		//a copy of the refnum if this node references a behavior object embedded in a smart entity.
		ESM::RefNum mRefNum;
	
		//The cost of choosing this node
		int mCost = 0;
		
		//Request a behavior object from this node
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

			mBehaviorObjectTemplate = behaviorobject;

			mRefNum = refnum;

			mCost = cost;

			mId = debugid;

		}

		//constructor for objects with multiple inputs and outputs
		GOAPNodeData(statusstack input, statusstack output, MWBase::BehaviorObject * behaviorobject, ESM::RefNum refnum, int cost, std::string debugid) {

			mInputs = input;

			mOutputs = output;

			mBehaviorObjectTemplate = behaviorobject;

			mRefNum = refnum;

			mCost = cost;

			mId = debugid;

		};
	};

	class BehaviorObject
	{
	protected:
		//ID used for debugging
		std::string mID;

		//Sometimes a behavior object needs to call on a sub behavior object, like a useSEI getting a BO from the SEI.
		BehaviorObject * mSubBehavior;

		//Keeps track of whether or not an NPC is getting behavior controlle by this BO or a subBO belonging to this BO
		bool mUsingSubBehavior = false;

		//if this bo targets something we store its refnum for sensorystore lookup
		ESM::RefNum mTargetRefNum;

		//owner life pointer, useful
		MWBase::Life * mOwnerLife;
	
		SmartEntityInstance * mSEITarget;
		
		//most bos require npc to go somewhere, this is for when npc is travelling
		bool mInJourney = false;
		
		//has the bo ben requested by ownerlife to stop?
		bool mStopRequested = false;

		MWBase::SubBrain * mParentSubBrain;

	public:
		 
		//To get a BO, a template BO is cloned. Template BOs are instantiated by their owner subbrains or SEIs.
		virtual BehaviorObject* Clone(MWBase::Life * life, ESM::RefNum refnum) = 0;

		virtual void getDebugInfo() = 0;

		void setOwner(MWBase::Life * life)
		{
			mOwnerLife = life;
		}

		virtual ~BehaviorObject() {

		};

	
		//Called when a life wants to first begin a BO
		virtual BOReturn start() = 0;

		//Called when a life has started a BO and wants to continue doing it
		virtual BOReturn update(float time, MWWorld::Ptr ownerptr) = 0;

		void setTarget(SmartEntityInstance * sei)
		{
			mSEITarget = sei;
		}

		//Called when a life wants to cancel the BO
		virtual bool stop() = 0;

		virtual void setParentSubBrain(MWBase::SubBrain * sb)
		{
			mParentSubBrain = sb;
		}

		
		
	};
		
	
	//A desire is a GOAPStatus with extra info: intensity and intention (how much NPC wants to make the status happen, and has the NPC decided to act on this desire)
	struct GOAPDesire
	{
		//What an NPC wants to change/do
		MWBase::GOAPStatus mStatus;
		
		//How badly it wants to change it
		int mIntensity;

		//When a desire is selected to be acted upon, it is considered an intention
		bool mIsIntention;

		std::string debugInfo = "desire template";
		
		
		GOAPDesire::GOAPDesire(MWBase::GOAPStatus stat, int val)
		{
			mStatus = stat;
			mIntensity = val;
			mIsIntention = false;
		};

		GOAPDesire::GOAPDesire()
		{

		};

		//Used for organizing desires from strongest to least strong
		bool operator> (const GOAPDesire& gd2) {
			return mIntensity > gd2.mIntensity;
		}

	};
		

	class SubBrain //used by NPCs to make decisions and solve problems
	{

	protected:

		MWBase::Life * mOwnerLife;

	public:
		
		virtual ~SubBrain() {

		};
		
		//Subbrain takes in the NPCs awareness, and from that calculates the NPCs desires
		virtual void calculate(MWBase::Awareness * awareness) = 0;



		//Used for debugging
		virtual std::string getID() = 0;

		//Takes in a desired status, returns a node linked to a BO that if accomplished will cause desired status change.
		virtual std::vector<std::shared_ptr<GOAPNodeData>> getMatchingBehaviorObjects(MWBase::GOAPStatus);

		virtual void getDebugInfo() = 0;

		virtual void clearDesires()
		{

		}

	};

	

}


#endif