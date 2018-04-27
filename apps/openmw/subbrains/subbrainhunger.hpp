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

		//virtual std::vector<std::shared_ptr<GOAPDesire>> getGOAPDesires();



	//	virtual std::vector<MWBase::BehaviorObject> getDesires();

		//virtual std::vector<BehaviorObject> getDesires();


	};

	class BOEat : public BehaviorObject
	{
		
	
	public:

		//BOEat::BOEat();

		BOEat* Clone()
		{
			return new BOEat(*this);
		};


		BOEat::BOEat(int valence);

		//void getDebugInfo();

		virtual void getDebugInfo();

		virtual BOReturn update(float time, MWWorld::Ptr ownerptr);

		virtual BOReturn start();
		
		virtual ~BOEat() {

		};

		virtual bool stop()
		{
			//by default no stop logic;
			std::cout << "BO with no stop logic recieved stop request" << std::endl;
			return false;
		};


	private:

		BOEat(BOEat const &instance)
		{
			

		};

	};

}



#endif