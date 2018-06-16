#ifndef GAME_GLSUBBRAINS_SUBBRAINFIGHTORFLIGHT_H
#define GAME_GLSUBBRAINS_SUBBRAINFIGHTORFLIGHT_H
#include <components/esm/defs.hpp>
#include "subbrain.hpp"
#include <string>

namespace MWBase {

	class Awareness;
	class BOFight;
	class BOFlee;
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

		BOFight * mFightBO;

		BOFlee * mFleeBO;


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

		virtual bool stop();

	private:


		BOFight(BOFight const &instance)
		{
			std::cout << "cloned get BO" << std::endl;
		};

	};


	class BOFlee : public MWBase::BehaviorObject {


	public:


		BOFlee();

		BOFlee* Clone(MWBase::Life * life, ESM::RefNum refnum)
		{
			BOFlee * newbo = new BOFlee(*this);
			newbo->mOwnerLife = life;
			newbo->mTargetRefNum = refnum;
			return newbo;
		}


		virtual BOReturn update(float time, MWWorld::Ptr ownerptr);

		virtual void getDebugInfo()
		{};

		ESM::Position mExpectedPosition;

		virtual BOReturn start();


		virtual ~BOFlee()
		{

		};

		virtual bool stop();

	private:


		BOFlee(BOFight const &instance)
		{
			std::cout << "cloned flee BO" << std::endl;
		};

	};


}



#endif

