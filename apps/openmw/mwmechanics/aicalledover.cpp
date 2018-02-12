#include "aicalledover.hpp"

#include <components/esm/aisequence.hpp>
#include <components/esm/loadcell.hpp>

#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"
#include "../mwbase/mechanicsmanager.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/cellstore.hpp"

#include "creaturestats.hpp"
#include "movement.hpp"
#include "steering.hpp"

#include "../mwworld/ptr.hpp"
#include "../mwworld/actiontalk.hpp"

namespace MWMechanics
{


	struct AiCalledOverStorage : AiTemporaryBase
	{
		float mTimer;
		bool mMoving;
		float mTargetAngleRadians;
		bool mTurnActorToTarget;

		AiCalledOverStorage() :
			mTimer(0.f),
			mMoving(false),
			mTargetAngleRadians(0.f),
			mTurnActorToTarget(false)
	
		{}
	};

	int AiCalledOver::mFollowIndexCounter = 0;

	AiCalledOver::AiCalledOver(const std::string &actorId, float duration, float x, float y, float z)
		: mAlwaysFollow(false), mCommanded(false), mDuration(duration), mRemainingDuration(duration), mX(x), mY(y), mZ(z)
		, mActorRefId(actorId), mActorId(-1), mCellId(""), mActive(false), mFollowIndex(mFollowIndexCounter++)
	{
	}

	AiCalledOver::AiCalledOver(const std::string &actorId, const std::string &cellId, float duration, float x, float y, float z)
		: mAlwaysFollow(false), mCommanded(false), mDuration(duration), mRemainingDuration(duration), mX(x), mY(y), mZ(z)
		, mActorRefId(actorId), mActorId(-1), mCellId(cellId), mActive(false), mFollowIndex(mFollowIndexCounter++)
	{
	}

	AiCalledOver::AiCalledOver(const std::string &actorId, bool commanded)
		: mAlwaysFollow(true), mCommanded(commanded), mDuration(0), mRemainingDuration(0), mX(0), mY(0), mZ(0)
		, mActorRefId(actorId), mActorId(-1), mCellId(""), mActive(false), mFollowIndex(mFollowIndexCounter++)
	{
	}

	AiCalledOver::AiCalledOver(const ESM::AiSequence::AiCalledOver *follow)
		: mAlwaysFollow(follow->mAlwaysFollow), mCommanded(follow->mCommanded), mRemainingDuration(follow->mRemainingDuration)
		, mX(follow->mData.mX), mY(follow->mData.mY), mZ(follow->mData.mZ)
		, mActorRefId(follow->mTargetId), mActorId(-1)
		, mCellId(follow->mCellId), mActive(follow->mActive), mFollowIndex(mFollowIndexCounter++)
	{
		// mDuration isn't saved in the save file, so just giving it "1" for now if the package had a duration.
		// The exact value of mDuration only matters for repeating packages.
		if (mRemainingDuration > 0) // Previously mRemainingDuration could be negative even when mDuration was 0. Checking for > 0 should fix old saves.
			mDuration = 1;
		else
			mDuration = 0;
	}

	bool AiCalledOver::execute(const MWWorld::Ptr& actor, CharacterController& characterController, AiState& state, float duration)
	{
		MWWorld::Ptr target = getTarget();

		if (target.isEmpty() || !target.getRefData().getCount() || !target.getRefData().isEnabled()  // Really we should be checking whether the target is currently registered
																									 // with the MechanicsManager
			)
			return false; // Target is not here right now, wait for it to return

		actor.getClass().getCreatureStats(actor).setDrawState(DrawState_Nothing);

		AiCalledOverStorage& storage = state.get<AiCalledOverStorage>();

		bool& rotate = storage.mTurnActorToTarget;
		if (rotate)
		{
			if (zTurn(actor, storage.mTargetAngleRadians))
				rotate = false;

			return false;
		}

		// AiFollow requires the target to be in range and within sight for the initial activation
		if (!mActive)
		{
			storage.mTimer -= duration;

			if (storage.mTimer < 0)
			{
				if ((actor.getRefData().getPosition().asVec3() - target.getRefData().getPosition().asVec3()).length2()
					< 10000 * 10000 //500*500
					&& MWBase::Environment::get().getWorld()->getLOS(actor, target))
					mActive = true;
				storage.mTimer = 0.5f;
			}
		}
		if (!mActive)
			return false;

		ESM::Position pos = actor.getRefData().getPosition(); //position of the actor

															  // The distances below are approximations based on observations of the original engine.
															  // If only one actor is following the target, it uses 186.
															  // If there are multiple actors following the same target, they form a group with each group member at 313 + (130 * i) distance to the target.

		short followDistance = 186;
		std::list<int> followers = MWBase::Environment::get().getMechanicsManager()->getActorsFollowingIndices(target);
		if (followers.size() >= 2)
		{
			followDistance = 313;
			short i = 0;
			followers.sort();
			for (std::list<int>::iterator it = followers.begin(); it != followers.end(); ++it)
			{
				if (*it == mFollowIndex)
					followDistance += 130 * i;
				++i;
			}
		}

		
			//We are near player, time to chat
			if ((actor.getRefData().getPosition().asVec3() - target.getRefData().getPosition().asVec3()).length2()
				< 200 * 200
				&& MWBase::Environment::get().getWorld()->getLOS(actor, target))
			{
				//MWBase::Environment::get().getWorld()->activate(target, actor);
				return true;
			}
			
			
		
			
		

		//Set the target destination from the actor
		ESM::Pathgrid::Point dest = target.getRefData().getPosition().pos;

		short baseFollowDistance = followDistance;
		short threshold = 30; // to avoid constant switching between moving/stopping
		if (storage.mMoving)
			followDistance -= threshold;
		else
			followDistance += threshold;

		osg::Vec3f targetPos(target.getRefData().getPosition().asVec3());
		osg::Vec3f actorPos(actor.getRefData().getPosition().asVec3());

		osg::Vec3f dir = targetPos - actorPos;
		float targetDistSqr = dir.length2();

		if (targetDistSqr <= followDistance * followDistance)
		{
			float faceAngleRadians = std::atan2(dir.x(), dir.y());

			if (!zTurn(actor, faceAngleRadians, osg::DegreesToRadians(45.f)))
			{
				storage.mTargetAngleRadians = faceAngleRadians;
				storage.mTurnActorToTarget = true;
			}

			return false;
		}

		storage.mMoving = !pathTo(actor, dest, duration, baseFollowDistance); // Go to the destination

		if (storage.mMoving)
		{
			//Check if you're far away
			float dist = distance(dest, pos.pos[0], pos.pos[1], pos.pos[2]);

			if (dist > 450)
				actor.getClass().getCreatureStats(actor).setMovementFlag(MWMechanics::CreatureStats::Flag_Run, true); //Make NPC run
			else if (dist < 325) //Have a bit of a dead zone, otherwise npc will constantly flip between running and not when right on the edge of the running threshold
				actor.getClass().getCreatureStats(actor).setMovementFlag(MWMechanics::CreatureStats::Flag_Run, true); //make NPC walk
		}

		return false;
	}

	std::string AiCalledOver::getFollowedActor()
	{
		return mActorRefId;
	}

	AiCalledOver *MWMechanics::AiCalledOver::clone() const
	{
		return new AiCalledOver(*this);
	}

	int AiCalledOver::getTypeId() const
	{
		return TypeIdCalledOver; //MWX
	}

	bool AiCalledOver::isCommanded() const
	{
		return mCommanded;
	}

	void AiCalledOver::writeState(ESM::AiSequence::AiSequence &sequence) const
	{
		std::unique_ptr<ESM::AiSequence::AiFollow> follow(new ESM::AiSequence::AiFollow());
		follow->mData.mX = mX;
		follow->mData.mY = mY;
		follow->mData.mZ = mZ;
		follow->mTargetId = mActorRefId;
		follow->mRemainingDuration = mRemainingDuration;
		follow->mCellId = mCellId;
		follow->mAlwaysFollow = mAlwaysFollow;
		follow->mCommanded = mCommanded;
		follow->mActive = mActive;

		ESM::AiSequence::AiPackageContainer package;
		package.mType = ESM::AiSequence::Ai_Follow;
		package.mPackage = follow.release();
		sequence.mPackages.push_back(package);
	}

	MWWorld::Ptr AiCalledOver::getTarget() const
	{
		if (mActorId == -2)
			return MWWorld::Ptr();

		if (mActorId == -1)
		{
			MWWorld::Ptr target = MWBase::Environment::get().getWorld()->searchPtr(mActorRefId, false);
			if (target.isEmpty())
			{
				mActorId = -2;
				return target;
			}
			else
				mActorId = target.getClass().getCreatureStats(target).getActorId();
		}

		if (mActorId != -1)
			return MWBase::Environment::get().getWorld()->searchPtrViaActorId(mActorId);
		else
			return MWWorld::Ptr();
	}

	int AiCalledOver::getFollowIndex() const
	{
		return mFollowIndex;
	}

	void AiCalledOver::fastForward(const MWWorld::Ptr& actor, AiState &state)
	{
		// Update duration counter if this package has a duration
		if (mDuration > 0)
			mRemainingDuration--;
	}

}
