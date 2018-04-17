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

		virtual std::vector<GOAPData*> getMatchingBehaviorObjects(MWBase::GOAPStatus);

	};

	class BOGetFromWorld : public MWBase::BehaviorObject {

	public:

		BOGetFromWorld::BOGetFromWorld();

		virtual void getDebugInfo();

		virtual ~BOGetFromWorld()
		{
			
		};

	};





}



#endif