#include "statusobjects.hpp"'
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/world.hpp"

void MWBase::FloatStatusObject::update(float duration)
{

	mTotalTime += duration;

	if (mTotalTime > 7.0)
	{
		end();
		return;
	}

	MWBase::Environment::get().getWorld()->queueMovement(mSEI->getPtr(), osg::Vec3f(100.f, 0.f, 600.f));
	//std::cout << "updating float" << std::endl;

}

void MWBase::FloatStatusObject::init()
{
	MWBase::Environment::get().getWorld()->addPhysicsActor(mSEI->getPtr());
	mSEI->getStatusManager()->mStatusMap.push_back(MWBase::FloatShroomPowdered);
}

void MWBase::FloatStatusObject::end()
{
	mSEI->getStatusManager()->removeStatus(MWBase::FloatShroomPowdered);
	mDone = true;
}
