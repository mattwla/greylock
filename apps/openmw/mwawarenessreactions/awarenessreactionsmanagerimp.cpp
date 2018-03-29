#include "awarenessreactionsmanagerimp.hpp"


#include <limits.h>
#include <iostream>
#include <fstream>

#include <components/misc/rng.hpp>

#include <components/esm/esmwriter.hpp>
#include <components/esm/stolenitems.hpp>

#include <components/sceneutil/positionattitudetransform.hpp>

#include "../mwworld/esmstore.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/player.hpp"
#include "../mwworld/ptr.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwworld/timestamp.hpp"
#include "../mwbase/lifemanager.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aiface.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/pathgrid.hpp"
#include "../mwmechanics/steering.hpp"


#include "../mwworld/player.hpp"
#include "../mwworld/manualref.hpp"
#include "../mwworld/cellstore.hpp"
#include "../mwworld/containerstore.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/actionteleport.hpp"
#include "../mwworld/projectilemanager.hpp"
#include "../mwworld/weather.hpp"
#include "../mwworld/scene.hpp"
#include "../mwworld/worldimp.cpp"
#include "../mwbase/statusmanager.hpp"
#include "../mwtasks/task.hpp"
#include "../mwtasks/fight.hpp"
#include "../mwtasks/confronttrespasser.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>

#include <osg/Group>
#include <osg/ComputeBoundsVisitor>


namespace MWAwarenessReactions
{
	AwarenessReactionsManager::AwarenessReactionsManager()
	{

	}


	//std::vector<MWWorld::Ptr> AwarenessReactionsManager::calculateAwareness(MWWorld::Ptr ptr)
	//{
	//	//std::vector<MWWorld::Ptr> vec;
	//	//unsigned int idx = 0;
	//	//while (idx < mLiveCellAffordances.size())
	//	//{

	//	//	if(MWBase::Environment::get().getWorld()->getLOS(ptr, mLiveCellAffordances[idx]) && awarenessCheck(mLiveCellAffordances[idx], ptr) && mLiveCellAffordances[idx] != ptr)
	//	//	{
	//	//		vec.push_back(mLiveCellAffordances[idx]);
	//	//	}
	//	//	idx += 1;
	//	//}

	//	//mNpcAwareOf[ptr] = vec; //mwx fix me massive memory leak here, updating ptr means massive bloat.
	//	//return vec;
	//}

	//bool AwarenessReactionsManager::awarenessCheck(const MWWorld::Ptr &ptr, const MWWorld::Ptr &observer)
	//{
	//	if (observer.getClass().getCreatureStats(observer).isDead() || !observer.getRefData().isEnabled())
	//		return false;
	//	osg::Vec3f pos1(ptr.getRefData().getPosition().asVec3());
	//	osg::Vec3f pos2(observer.getRefData().getPosition().asVec3());
	//	// is ptr behind the observer?
	//	osg::Vec3f vec = pos1 - pos2;
	//	if (observer.getRefData().getBaseNode())
	//	{
	//		osg::Vec3f observerDir = (observer.getRefData().getBaseNode()->getAttitude() * osg::Vec3f(0, 1, 0));

	//		float angleRadians = std::acos(observerDir * vec / (observerDir.length() * vec.length()));
	//		if (angleRadians > osg::DegreesToRadians(90.f))
	//			return false;
	//		else
	//			return true;
	//	}

	//	std::cout << "couldnt get basenode for awareness check" << std::endl;
	//	return false;
	//}



	//bool AwarenessReactionsManager::turnTo(MWWorld::Ptr actor, MWWorld::Ptr target) {
	//
	//	osg::Vec3f targetPos(target.getRefData().getPosition().asVec3());
	//	osg::Vec3f actorPos(actor.getRefData().getPosition().asVec3());

	//	osg::Vec3f dir = targetPos - actorPos;

	//	float faceAngleRadians = std::atan2(dir.x(), dir.y());

	//	if (true)
	//	{
	//		if (MWMechanics::zTurn(actor, faceAngleRadians))
	//			bool rotate = false;
	//		else
	//			return false;
	//	}
	//	
	//}

}