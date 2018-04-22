#ifndef GAME_GLSUBBRAINS_SUBBRAINRELAX_H
#define GAME_GLSUBBRAINS_SUBBRAINRELAX_H
#include <components/esm/defs.hpp>
#include "subbrain.hpp"
#include <string>

namespace MWBase{

	class Awareness;
}


namespace MWBase {

	class SubBrainRelax : public MWBase::SubBrain {

		//BehaviorObject * mEatBO;

	public:

		SubBrainRelax::SubBrainRelax(MWBase::Life * life);

		virtual ~SubBrainRelax() {

		};

		virtual void calculate(MWBase::Awareness * awareness);

		virtual std::string getID();

		virtual void getDebugInfo();

	//	virtual std::vector<MWBase::BehaviorObject> getDesires();

		//virtual std::vector<BehaviorObject> getDesires();


	};



}



#endif