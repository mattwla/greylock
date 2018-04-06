#ifndef GAME_GLSUBBRAINS_SUBBRAIN_H
#define GAME_GLSUBBRAINS_SUBBRAIN_H
#include <components/esm/defs.hpp>
#include <string>
#include <vector>
#include "../mwworld/ptr.hpp"

namespace MWBase {
	class Life;
	class Awareness;
	struct GOAPStatus;
	struct GOAPData;
};

namespace MWWorld {
	class Ptr;
}

namespace ESM {
	class RefNum;
}



namespace MWBase {

	typedef std::vector<GOAPStatus> statusstack;

	struct GOAPData
	{
		statusstack mInputs;
		statusstack mOutputs;

		//getcost
		//check inputs valid (pass ref to statuslist)
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
		GOAPData mGOAPData;

	public:
	/*	bool isDesire()
		{
			return mIsDesire;
		};*/

		virtual void getDebugInfo() = 0;

		virtual ~BehaviorObject() {

		};



			//start
			//pause
			//run
	};
		
	struct GOAPStatus
	{
		enum StatusType {
			HAS_IN_INVENTORY = 0,
			VITALS = 1,

		};

		StatusType mStatusType;
		std::string mExtraData;
		int mAmount;

		GOAPStatus::GOAPStatus(StatusType type, std::string extradata, int amount)
		{
			mStatusType = type;
			mExtraData = extradata;
			mAmount = amount;
		};
	};
		




	class SubBrain {

	protected:

		MWBase::Life * mOwnerLife;

		std::vector<GOAPStatus> mBeliefs;
		//Beliefs
		
		std::vector<BehaviorObject*> mDesires;
		//Desires

		
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

		virtual void getDebugInfo() = 0;

};

}
#include <string>


#endif