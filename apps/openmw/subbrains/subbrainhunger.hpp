#ifndef GAME_GLSUBBRAINS_SUBBRAINHUNGER_H
#define GAME_GLSUBBRAINS_SUBBRAINHUNGER_H
#include <components/esm/defs.hpp>
#include "subbrain.hpp"
#include <string>
#include <memory>

namespace MWBase{

	class Awareness;
}


namespace MWBase {

	class SubBrainHunger : public MWBase::SubBrain {

		BehaviorObject * mEatBO;

		std::shared_ptr<GOAPDesire> mHungerDesire;

	public:

		SubBrainHunger::SubBrainHunger(MWBase::Life * life);

		virtual ~SubBrainHunger() {

		};

		virtual void calculate(MWBase::Awareness * awareness);

		virtual std::string getID();

		virtual void getDebugInfo();

		virtual std::vector<std::shared_ptr<GOAPNodeData>> getMatchingBehaviorObjects(MWBase::GOAPStatus status);


	};

	class BOEat : public BehaviorObject
	{
		
	
	public:

		//BOEat::BOEat();

		BOEat* Clone(MWBase::Life * life, ESM::RefNum refnum)
		{
			BOEat * newbo = new BOEat(*this);
			newbo->mOwnerLife = life;
			newbo->mTargetRefNum = refnum;
			return newbo;
		};


		BOEat::BOEat(int intensity);

		//void getDebugInfo();

		virtual void getDebugInfo();

		virtual BOReturn update(float time, MWWorld::Ptr ownerptr);

		virtual BOReturn start();
		
		virtual ~BOEat() {

		};

		virtual bool stop()
		{
			mStopRequested = true;
			//by default no stop logic;
			std::cout << "BO with no stop logic recieved stop request - BOEAT" << std::endl;
			return true;
		};


	private:

		BOEat(BOEat const &instance)
		{
			

		};

	};

}



#endif