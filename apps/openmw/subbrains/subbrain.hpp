#ifndef GAME_GLSUBBRAINS_SUBBRAIN_H
#define GAME_GLSUBBRAINS_SUBBRAIN_H
#include <components/esm/defs.hpp>
#include <string>
#include "../mwworld/ptr.hpp"

namespace MWBase {
	class Life;
	class Awareness;
};

namespace MWWorld {
	class Ptr;
}

namespace ESM {
	class RefNum;
}


namespace MWBase {




	class BehaviorObject
	{
		std::string mID;
		BehaviorObject * mSubBehavior;
		MWWorld::Ptr mOwnerPtr;
		ESM::RefNum mOwnerRefNum;

	public:
		
		struct GOAPData
		{
			//getcost
			//input
			//output
		};


	};


	class SubBrain {

	protected:

		MWBase::Life * mOwnerLife;

		//A store of subbrains?

	public:
		
		virtual void calculate(MWBase::Awareness * awareness) = 0;

		virtual std::string getID() = 0;

};

}
#include <string>


#endif