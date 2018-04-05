#ifndef GAME_GLSUBBRAINS_SUBBRAININVENTORY_H
#define GAME_GLSUBBRAINS_SUBBRAININVENTORY_H
#include <components/esm/defs.hpp>
#include "subbrain.hpp"
#include <string>

namespace MWBase {

	class Awareness;
}


namespace MWBase {

	class SubBrainInventory : public MWBase::SubBrain {

	public:

		SubBrainInventory::SubBrainInventory(MWBase::Life * life);

		virtual void calculate(MWBase::Awareness * awareness);

		virtual std::string getID();

	};



}



#endif