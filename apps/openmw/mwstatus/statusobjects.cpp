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

	//MWBase::Environment::get().getWorld()->queueMovement(mSEI->getPtr(), osg::Vec3f(100.f, 0.f, 600.f));
	auto pos = mSEI->getPtr().getRefData().getPosition().pos;
	MWBase::Environment::get().getWorld()->moveObject(mSEI->getPtr(), pos[0], pos[1], pos[2]+1.0f );
	//std::cout << "updating float" << std::endl;

}

void MWBase::FloatStatusObject::init()
{
	//MWBase::Environment::get().getWorld()->addPhysicsActor(mSEI->getPtr());
	mSEI->getStatusManager()->mStatusMap.push_back(MWBase::FloatShroomPowdered);
}

void MWBase::FloatStatusObject::end()
{
	mSEI->getStatusManager()->removeStatus(MWBase::FloatShroomPowdered);
	mDone = true;
}
