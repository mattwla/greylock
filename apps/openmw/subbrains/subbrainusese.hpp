#ifndef GAME_GLSUBBRAINS_SUBBRAINUSESE_H
#define GAME_GLSUBBRAINS_SUBBRAINUSESE_H
#include <components/esm/defs.hpp>
#include "subbrain.hpp"
#include <string>

namespace MWBase {

	class Awareness;
}


namespace MWBase {

	class SubBrainUseSE : public MWBase::SubBrain {

	public:

		SubBrainUseSE::SubBrainUseSE(MWBase::Life * life);

		virtual ~SubBrainUseSE() {

		};

		virtual void calculate(MWBase::Awareness * awareness);

		virtual std::string getID();

		virtual void getDebugInfo();

		BehaviorObject * mUseSEInWorldBO;

		virtual std::vector<std::shared_ptr<GOAPNodeData>> getMatchingBehaviorObjects(MWBase::GOAPStatus);

		int getCost(SmartEntityInstance * sei); //what of for a different BO?

		bool InUsingRange(MWBase::SmartEntityInstance * sei);

	};

	class BOUseSEInWorld : public MWBase::BehaviorObject {

		

	public:


		BOUseSEInWorld::BOUseSEInWorld();

		BOUseSEInWorld* Clone(MWBase::Life * life, ESM::RefNum refnum)
		{
			BOUseSEInWorld * newbo = new BOUseSEInWorld(*this);
			newbo->mOwnerLife = life;
			newbo->mTargetRefNum = refnum;
			
			return newbo;
		}

	
		virtual BOReturn update(float time, MWWorld::Ptr ownerptr);

		bool checkSEUnavailableNotice();

		void useSE();

		virtual void getDebugInfo();

		bool inUsingDistance();

		ESM::Position mExpectedPosition;

		virtual BOReturn start();


		virtual ~BOUseSEInWorld()
		{
			
		};

		virtual bool stop()
		{
			std::cout << "use se bo got stop request" << std::endl;

			mStopRequested = true;
			//by default no stop logic;
			if (mUsingSubBehavior)
				return mSubBehavior->stop();
			else
				return true;
		};


	private:

		BOUseSEInWorld(BOUseSEInWorld const &instance)
		{
			std::cout << "cloned useSEinworld BO" << std::endl;
		};

	};





}



#endif

