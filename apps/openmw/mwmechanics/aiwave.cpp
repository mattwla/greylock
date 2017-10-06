#include "aiwave.hpp"

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

namespace MWMechanics
{


	struct AiFollowStorage : AiTemporaryBase
	{
		float mTimer;
		bool mMoving;
		float mTargetAngleRadians;
		bool mTurnActorToTarget;

		AiFollowStorage() :
			mTimer(0.f),
			mMoving(false),
			mTargetAngleRadians(0.f),
			mTurnActorToTarget(false)
		{}
	};

	int AiWave::mFollowIndexCounter = 0;

	AiWave::AiWave(const std::string &actorId, float duration, float x, float y, float z)
		: mAlwaysFollow(false), mCommanded(false), mDuration(duration), mRemainingDuration(duration), mX(x), mY(y), mZ(z)
		, mActorRefId(actorId), mActorId(-1), mCellId(""), mActive(false), mFollowIndex(mFollowIndexCounter++)
	{
	}

	AiWave::AiWave(const std::string &actorId, const std::string &cellId, float duration, float x, float y, float z)
		: mAlwaysFollow(false), mCommanded(false), mDuration(duration), mRemainingDuration(duration), mX(x), mY(y), mZ(z)
		, mActorRefId(actorId), mActorId(-1), mCellId(cellId), mActive(false), mFollowIndex(mFollowIndexCounter++)
	{
	}

	AiWave::AiWave(const std::string &actorId, bool commanded)
		: mAlwaysFollow(true), mCommanded(commanded), mDuration(0), mRemainingDuration(0), mX(0), mY(0), mZ(0)
		, mActorRefId(actorId), mActorId(-1), mCellId(""), mActive(false), mFollowIndex(mFollowIndexCounter++)
	{
	}

	AiWave::AiWave(const ESM::AiSequence::AiWave *follow)
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

	bool AiWave::execute(const MWWorld::Ptr& actor, CharacterController& characterController, AiState& state, float duration)
	{
		

		//MXW
		
		MWWorld::Ptr target = getTarget();

		if (target.isEmpty() || !target.getRefData().getCount() || !target.getRefData().isEnabled()  // Really we should be checking whether the target is currently registered
																									 // with the MechanicsManager
			)
			return false; // Target is not here right now, wait for it to return

		actor.getClass().getCreatureStats(actor).setDrawState(DrawState_Nothing);

		AiFollowStorage& storage = state.get<AiFollowStorage>();
		osg::Vec3f targetPos(target.getRefData().getPosition().asVec3());
		osg::Vec3f actorPos(actor.getRefData().getPosition().asVec3());

		osg::Vec3f dir = targetPos - actorPos;

		float faceAngleRadians = std::atan2(dir.x(), dir.y());
		bool& rotate = storage.mTurnActorToTarget;
		if (true)
		{
			if (zTurn(actor, faceAngleRadians))
				rotate = false;
			else
				return false;
		}
		//MWX wow.

		//MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(actor, "wave", 0, 1);
		actor.getClass().getMovementSettings(actor).mPosition[1] = 0;
		
		if (!MWBase::Environment::get().getMechanicsManager()->checkAnimationPlaying(actor, "wave") && storage.mTimer == 0.f)
		{
			MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(actor, "wave", 0, 1);
			storage.mTimer = 1.0f;
		}
		else if (!MWBase::Environment::get().getMechanicsManager()->checkAnimationPlaying(actor, "wave"))
			return true;
			
	
		return false;

		
		
		//MWX
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

		if (!mAlwaysFollow) //Update if you only follow for a bit
		{
			//Check if we've run out of time
			if (mDuration > 0)
			{
				mRemainingDuration -= ((duration*MWBase::Environment::get().getWorld()->getTimeScaleFactor()) / 3600);
				if (mRemainingDuration <= 0)
				{
					mRemainingDuration = mDuration;
					return true;
				}
			}

			if ((pos.pos[0] - mX)*(pos.pos[0] - mX) +
				(pos.pos[1] - mY)*(pos.pos[1] - mY) +
				(pos.pos[2] - mZ)*(pos.pos[2] - mZ) < followDistance*followDistance) //Close-ish to final position
			{
				if (actor.getCell()->isExterior()) //Outside?
				{
					if (mCellId == "") //No cell to travel to
						return true;
				}
				else
				{
					if (mCellId == actor.getCell()->getCell()->mName) //Cell to travel to
						return true;
				}
			}

			
		}

		//Set the target destination from the actor
		ESM::Pathgrid::Point dest = target.getRefData().getPosition().pos;

		short baseFollowDistance = followDistance;
		short threshold = 30; // to avoid constant switching between moving/stopping
		if (storage.mMoving)
			followDistance -= threshold;
		else
			followDistance += threshold;

		//osg::Vec3f targetPos(target.getRefData().getPosition().asVec3());
		//osg::Vec3f actorPos(actor.getRefData().getPosition().asVec3());

//		osg::Vec3f dir = targetPos - actorPos;
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
				actor.getClass().getCreatureStats(actor).setMovementFlag(MWMechanics::CreatureStats::Flag_Run, false); //make NPC walk
		}
		


		return false;
	}

	std::string AiWave::getFollowedActor()
	{
		return mActorRefId;
	}

	AiWave *MWMechanics::AiWave::clone() const
	{
		return new AiWave(*this);
	}

	int AiWave::getTypeId() const
	{
		return TypeIdWave;
	}

	bool AiWave::isCommanded() const
	{
		return mCommanded;
	}

	void AiWave::writeState(ESM::AiSequence::AiSequence &sequence) const
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

	MWWorld::Ptr AiWave::getTarget() const
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

	int AiWave::getFollowIndex() const
	{
		return mFollowIndex;
	}

	void AiWave::fastForward(const MWWorld::Ptr& actor, AiState &state)
	{
		// Update duration counter if this package has a duration
		if (mDuration > 0)
			mRemainingDuration--;
	}

}
