#ifndef GAME_MWBASE_LIFEMANAGER_H
#define GAME_MWBASE_LIFEMANAGER_H

#include <string>
#include <vector>
#include <list>
#include <set>
#include <stdint.h>
#include <map>

#include <list>

#include <components/esm/loadpgrd.hpp>
#include "../mwmechanics/pathgrid.hpp"
#include "../mwworld/timestamp.hpp"

#include "../mwworld/ptr.hpp"
#include "../mwtasks/task.hpp"
#include "../mwbase/statusmanager.hpp"
#include "../mwbase/aischedulemanager.hpp"
#include "../mwtasks/life.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../subbrains/subbrain.hpp"



#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>

namespace osg
{
	class Vec3f;
}

namespace ESM
{
	struct Class;

	class ESMReader;
	class ESMWriter;
}

namespace MWWorld
{
	class Ptr;
	class CellStore;
	class CellRef;
}

namespace Loading
{
	class Listener;
}


namespace MWBase
{

	struct BigFive {
		int mOpenness;
		int mConscientiousness;
		int mExtraversion;
		int mAgreeableness;
		int mNeuroticism;
	};

	struct Vitals {

		float mHunger;
		float mSleepiness;

	};

	

	class SubBrainsManager
	{
		std::vector<SubBrain*> mSubBrains;


	public:

		void calculate(MWBase::Awareness * awareness);

		SubBrainsManager::SubBrainsManager(MWBase::Life * life);

		//SubBrainsManager::SubBrainsManager();
	

	};
	
	
	struct Life
	{
		std::string mId;

		int mRefNum;

		MWWorld::CellStore *mOwnerCell;

		MWBase::Awareness *mAwareness;

		MWBase::SubBrainsManager *mSubBrainsManager;

		MWWorld::Ptr mPtr;

		BigFive mBigFive;

		Vitals mVitals;

	public:

		Life(std::string id)
		{
			mPtr = MWBase::Environment::get().getWorld()->searchPtr(id, false);
			mId = id;
			mRefNum = mPtr.getCellRef().getRefNum().mIndex;
			mOwnerCell = mPtr.getCell();
			mPtr.getBase()->mLife = this;
			mAwareness = new MWBase::Awareness(mPtr);
			mSubBrainsManager = new MWBase::SubBrainsManager(this);
			
		}

		void getDebugInfo();

		void update(float duration);

	private:

		void metabolize(float duration);



	};

	class LifeManager
	{
		
	public:

		virtual void newGame() = 0;
		
		virtual void update(float duration, bool paused) = 0;

		virtual void initialize() = 0;

		virtual ~LifeManager() {}

		std::vector<MWBase::Life*> mLifeList;

		virtual bool inActiveRange(MWWorld::Ptr npc) = 0;

		float mLastTimeReported;

		float mTimePassed;

		float mTimeAccumulator;

		

	};

	
	
	
}
#endif
