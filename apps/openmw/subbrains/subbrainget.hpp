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

		virtual void calculate(MWBase::Awareness * awareness);

		virtual std::string getID();

		virtual void getDebugInfo();

	};

	class BOGet : public MWBase::BehaviorObject {

	public:
		virtual void getDebugInfo();

	};





}



#endif