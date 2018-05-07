#ifndef GAME_GLSUBBRAINS_SUBBRAINCIRCADIANRHYTHM_H
#define GAME_GLSUBBRAINS_SUBBRAINCIRCADIANRHYTHM_H
#include <components/esm/defs.hpp>
#include "subbrain.hpp"
#include <string>
#include <memory>

namespace MWBase{

	class Awareness;
}


namespace MWBase {

	class SubBrainCircadianRhythm : public MWBase::SubBrain {

		BehaviorObject * mSleepOnGroundBO;

		std::shared_ptr<GOAPDesire> mSleepDesire;

	public:

		SubBrainCircadianRhythm::SubBrainCircadianRhythm(MWBase::Life * life);

		virtual ~SubBrainCircadianRhythm() {

		};

		virtual void calculate(MWBase::Awareness * awareness);

		virtual std::string getID();

		virtual void getDebugInfo();

		virtual std::vector<std::shared_ptr<GOAPNodeData>> getMatchingBehaviorObjects(MWBase::GOAPStatus status);

		//virtual std::vector<std::shared_ptr<GOAPDesire>> getGOAPDesires();



	//	virtual std::vector<MWBase::BehaviorObject> getDesires();

		//virtual std::vector<BehaviorObject> getDesires();


	};

	class BOSleepOnGround : public BehaviorObject
	{
		
	
	public:

		//BOSleepOnGround::BOSleepOnGround();

		BOSleepOnGround* Clone(MWBase::Life * life, ESM::RefNum refnum)
		{
			BOSleepOnGround * newbo = new BOSleepOnGround(*this);
			newbo->mOwnerLife = life;
			newbo->mTargetRefNum = refnum;
			return newbo;
		};


		BOSleepOnGround::BOSleepOnGround(int valence);

		//void getDebugInfo();

		virtual void getDebugInfo();

		virtual BOReturn update(float time, MWWorld::Ptr ownerptr);

		virtual BOReturn start();
		
		virtual ~BOSleepOnGround() {

		};

		virtual bool stop()
		{
			mStopRequested = true;
			//by default no stop logic;
			//std::cout << "BO with no stop logic recieved stop request - sleeponground" << std::endl;
			return true;
		};


	private:

		BOSleepOnGround(BOSleepOnGround const &instance)
		{
			

		};

	};

}



#endif