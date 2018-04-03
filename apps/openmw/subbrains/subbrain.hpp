#ifndef GAME_GLSUBBRAINS_SUBBRAIN_H
#define GAME_GLSUBBRAINS_SUBBRAIN_H
#include <components/esm/defs.hpp>

namespace MWBase {
	class Life;
	class Awareness;
};


namespace MWBase {

	class SubBrain {

	protected:
		
		MWBase::Life * mOwnerLife;

	public:
		
		virtual void calculate(MWBase::Awareness * awareness) = 0;

};

}
#include <string>


#endif