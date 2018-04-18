#ifndef GAME_GLSUBBRAINS_SUBBRAINGET_H
#define GAME_GLSUBBRAINS_SUBBRAINGET_H
#include <components/esm/defs.hpp>
#include "subbrain.hpp"
#include <string>

namespace MWBase {

	class Awareness;
}


namespace MWBase {

	class SubBrainGet : public MWBase::SubBrain {

	public:

		SubBrainGet::SubBrainGet(MWBase::Life * life);

		virtual ~SubBrainGet() {

		};

		virtual void calculate(MWBase::Awareness * awareness);

		virtual std::string getID();

		virtual void getDebugInfo();

		BehaviorObject * mGetFromWorldBO;

		virtual std::vector<std::shared_ptr<GOAPData>> getMatchingBehaviorObjects(MWBase::GOAPStatus);

		int getCost(SmartEntityInstance * sei); //what of for a different BO?

		bool InGrabbingRange(MWBase::SmartEntityInstance * sei);

	};

	class BOGetFromWorld : public MWBase::BehaviorObject {

		

	public:


		BOGetFromWorld::BOGetFromWorld();

		BOGetFromWorld* Clone()
		{
			return new BOGetFromWorld(*this);
		}

	
		virtual BOReturn update(float time, MWWorld::Ptr ownerptr);

		virtual void getDebugInfo();

		bool inGrabbingDistance();


		virtual ~BOGetFromWorld()
		{
			
		};

	private:

		BOGetFromWorld(BOGetFromWorld const &instance) 
		{
			std::cout << "cloned get BO" << std::endl;
		};

	};





}



#endif

