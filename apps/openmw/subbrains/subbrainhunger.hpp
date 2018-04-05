#ifndef GAME_GLSUBBRAINS_SUBBRAINHUNGER_H
#define GAME_GLSUBBRAINS_SUBBRAINHUNGER_H
#include <components/esm/defs.hpp>
#include "subbrain.hpp"
#include <string>

namespace MWBase{

	class Awareness;
}


namespace MWBase {

	class SubBrainHunger : public MWBase::SubBrain {

	public:

		SubBrainHunger::SubBrainHunger(MWBase::Life * life);

		virtual void calculate(MWBase::Awareness * awareness);

		virtual std::string getID();

	};

	class BOEat : public BehaviorObject
	{
		
	
	public:

		BOEat::BOEat();




	};

}



#endif