#ifndef GAME_GLSUBBRAINS_SUBBRAINFIGHTORFLIGHT_H
#define GAME_GLSUBBRAINS_SUBBRAINFIGHTORFLIGHT_H
#include <components/esm/defs.hpp>
#include "subbrain.hpp"
#include <string>

namespace MWBase {

	class Awareness;
}


namespace MWBase {

	class SubBrainFightOrFlight : public MWBase::SubBrain {

	public:

		SubBrainFightOrFlight::SubBrainFightOrFlight(MWBase::Life * life);

		virtual ~SubBrainFightOrFlight() {

		};

		virtual void calculate(MWBase::Awareness * awareness);

		virtual std::string getID();

		virtual void getDebugInfo();

		BehaviorObject * mGetFromWorldBO;

		virtual std::vector<std::shared_ptr<GOAPNodeData>> getMatchingBehaviorObjects(MWBase::GOAPStatus);

		int getCost(SmartEntityInstance * sei); //what of for a different BO?


	};

	class BOFight : public MWBase::BehaviorObject {



	public:


		BOFight();

		BOFight* Clone(MWBase::Life * life, ESM::RefNum refnum)
		{
			BOFight * newbo = new BOFight(*this);
			newbo->mOwnerLife = life;
			newbo->mTargetRefNum = refnum;
			return newbo;
		}


		virtual BOReturn update(float time, MWWorld::Ptr ownerptr);

		virtual void getDebugInfo();

		ESM::Position mExpectedPosition;

		virtual BOReturn start();


		virtual ~BOFight()
		{

		};

		virtual bool stop()
		{
			//by default no stop logic;
			mStopRequested = true;
			std::cout << "BO with no stop logic recieved stop request.... FIGHT" << std::endl;
			return true;
		};

	private:

		BOFight(BOFight const &instance)
		{
			std::cout << "cloned get BO" << std::endl;
		};

	};


}



#endif

