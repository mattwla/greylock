#ifndef GAME_GLSUBBRAINS_SUBBRAIN_H
#define GAME_GLSUBBRAINS_SUBBRAIN_H
#include <components/esm/defs.hpp>
#include <string>

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

		virtual std::string getID() = 0;

};

}
#include <string>


#endif