#ifndef GAME_MWMECHANICS_CHARACTER_HPP
#define GAME_MWMECHANICS_CHARACTER_HPP

#include <deque>

#include <components/esm/loadmgef.hpp>

#include "../mwworld/ptr.hpp"
#include "../mwworld/containerstore.hpp"

#include "../mwrender/animation.hpp"

#include "../mwphysics/physicssystem.hpp"
#include "../mwphysics/actor.hpp"
#include "../mwphysics/collisiontype.hpp"


namespace MWWorld
{
    class InventoryStore;
}

namespace MWRender
{
    class Animation;
}

namespace MWMechanics
{

struct Movement;
class CreatureStats;

enum Priority {
    Priority_Default,
    Priority_WeaponLowerBody,
    Priority_SneakIdleLowerBody,
    Priority_SwimIdle,
    Priority_Jump,
    Priority_Movement,
    Priority_Hit,
    Priority_Weapon,
    Priority_Block,
    Priority_Knockdown,
    Priority_Torch,
    Priority_Storm,

    Priority_Death,

    Num_Priorities
};

enum CharacterState {
    CharState_None,

    CharState_SpecialIdle,
    CharState_Idle,
    CharState_Idle2,
    CharState_Idle3,
    CharState_Idle4,
    CharState_Idle5,
    CharState_Idle6,
    CharState_Idle7,
    CharState_Idle8,
    CharState_Idle9,
    CharState_IdleSwim,
    CharState_IdleSneak,

    CharState_WalkForward,
    CharState_WalkBack,
    CharState_WalkLeft,
    CharState_WalkRight,

    CharState_SwimWalkForward,
    CharState_SwimWalkBack,
    CharState_SwimWalkLeft,
    CharState_SwimWalkRight,

    CharState_RunForward,
    CharState_RunBack,
    CharState_RunLeft,
    CharState_RunRight,

    CharState_SwimRunForward,
    CharState_SwimRunBack,
    CharState_SwimRunLeft,
    CharState_SwimRunRight,

    CharState_SneakForward,
    CharState_SneakBack,
    CharState_SneakLeft,
    CharState_SneakRight,

    CharState_TurnLeft,
    CharState_TurnRight,
    CharState_SwimTurnLeft,
    CharState_SwimTurnRight,

    CharState_Jump,

    CharState_Death1,
    CharState_Death2,
    CharState_Death3,
    CharState_Death4,
    CharState_Death5,
    CharState_SwimDeath,
    CharState_SwimDeathKnockDown,
    CharState_SwimDeathKnockOut,
    CharState_DeathKnockDown,
    CharState_DeathKnockOut,

    CharState_Hit,
    CharState_SwimHit,
    CharState_KnockDown,
    CharState_KnockOut,
    CharState_SwimKnockDown,
    CharState_SwimKnockOut,
    CharState_Block
};

enum WeaponType {
    WeapType_None,

    WeapType_HandToHand,
    WeapType_OneHand,
    WeapType_TwoHand,
    WeapType_TwoWide,
    WeapType_BowAndArrow,
    WeapType_Crossbow,
    WeapType_Thrown,
    WeapType_PickProbe,

    WeapType_Spell
};

enum UpperBodyCharacterState {
    UpperCharState_Nothing,
    UpperCharState_EquipingWeap,
    UpperCharState_UnEquipingWeap,
    UpperCharState_WeapEquiped,
    UpperCharState_StartToMinAttack,
    UpperCharState_MinAttackToMaxAttack,
    UpperCharState_MaxAttackToMinHit,
    UpperCharState_MinHitToHit,
    UpperCharState_FollowStartToFollowStop,
    UpperCharState_CastingSpell
};

enum JumpingState {
    JumpState_None,
    JumpState_InAir,
    JumpState_Landing
};

//enum ClimbingState {
//	ClimbState_None,
//	ClimbState_Climbing
//};
//
//struct ClimbData {
//	float z = 0.f;
//	float originalz = 0.f;
//	float forward = 0.f;
//	float originalforward = 0.f;
//	osg::Vec3f direction;
//};

enum ActionState {
	ActionState_Climbing = 0,
	ActionState_WallHolding = 1,
	ActionState_Gliding = 2
};

struct ClimbData {
	float z;
	float reach;
	bool mFound;
};

class CharacterAction 
{
	

public:
	CharacterAction()
	{

	}
	
	virtual ~CharacterAction();

	float mTimer;

	MWWorld::Ptr mPtr;

	virtual bool update(float duration);

	virtual ActionState getType();

	bool mDone;
};

class Climb : public MWMechanics::CharacterAction
{

	float mTargetZ;
	float mTargetForward;
	osg::Vec3f mForwardDirection;
	bool mRotateStage;

public:
	Climb()
	{
	
	}

	Climb(MWWorld::Ptr ptr, float targetZ);

	virtual ~Climb();

	virtual bool update(float duration);

	virtual ActionState getType();

};

class Glide : public MWMechanics::CharacterAction
{
	enum TiltState
	{
		PLAYER_CONTROLLED = 0,
		INITIAL_RETURN_FROM_RIGHT = 1,
		INITIAL_RETURN_FROM_LEFT = 2,
		OVER_RETURN = 3,
		CORRECTIVE_RETURN = 4,
		RANDOM_DRIFT = 5,
		RANDOM_DRIFT_RETURN = 6
	};

	TiltState mTiltState;

	float mTargetRoll;

	float mDriftTimer;

	float mTiltOnRelease;

	public :

		Glide(MWWorld::Ptr ptr);

		virtual ~Glide();

		virtual bool update(float duration);

		virtual ActionState getType();


};

class WallHold : public MWMechanics::CharacterAction
{

public:

	

	//WallHold(MWWorld::Ptr ptr, osg::Vec3f originalvelocity);

	WallHold(MWWorld::Ptr ptr, osg::Vec3f originalvelocity, bool camswitch);

	virtual ~WallHold();



	virtual bool update(float duraction);

	virtual ActionState getType();

	int mWallHoldIdx;

	osg::Vec3f mOriginalVelocity;

	osg::Vec3f mStoredJump;
	
	bool mCamSwitch;

	bool mRotateStage;

	
};

struct WeaponInfo;

class CharacterController : public MWRender::Animation::TextKeyListener
{
    MWWorld::Ptr mPtr;
    MWRender::Animation *mAnimation;
    
    struct AnimationQueueEntry
    {
        std::string mGroup;
        size_t mLoopCount;
        bool mPersist;
    };

	enum RayDirection
	{
		forward = 0,
		up = 1,
		down = 2
	};
    typedef std::deque<AnimationQueueEntry> AnimationQueue;
    AnimationQueue mAnimQueue;

    CharacterState mIdleState;
    std::string mCurrentIdle;

    CharacterState mMovementState;
    std::string mCurrentMovement;
    float mMovementAnimSpeed;
    bool mAdjustMovementAnimSpeed;
    bool mHasMovedInXY;
    bool mMovementAnimationControlled;

    CharacterState mDeathState;
    std::string mCurrentDeath;
    bool mFloatToSurface;

    CharacterState mHitState;
    std::string mCurrentHit;

    UpperBodyCharacterState mUpperBodyState;

    JumpingState mJumpState;
    std::string mCurrentJump;

	CharacterAction* mCurrentAction;

	float mClimbTimer;
	bool mRotateStage;
	bool mLastSneakStatus = 0;

	bool mWallGrabCamSwitch;

    WeaponType mWeaponType;
    std::string mCurrentWeapon;

    float mAttackStrength;

    bool mSkipAnim;

    // counted for skill increase
    float mSecondsOfSwimming;
    float mSecondsOfRunning;
	
	bool landed;
	float mWallJumpCooldown;
	float mWallJumpRotation;
	float mWallJumpCameraTilt;
	bool mInWallJump;
	float mWallJumpPause;
	int mWallJumpIdx;
	float mWallJumpInitialTilt;
	osg::Vec3f mWallJumpOriginalVelocity;
	osg::Vec3f currentvelocity;
	float mBaseFov;

    MWWorld::ConstPtr mHeadTrackTarget;

    float mTurnAnimationThreshold; // how long to continue playing turning animation after actor stopped turning

    std::string mAttackType; // slash, chop or thrust

    bool mAttackingOrSpell;

    float mTimeUntilWake;

    void setAttackTypeBasedOnMovement();

    void refreshCurrentAnims(CharacterState idle, CharacterState movement, JumpingState jump, bool force=false);
    void refreshHitRecoilAnims();
    void refreshJumpAnims(const WeaponInfo* weap, JumpingState jump, bool force=false);
    void refreshMovementAnims(const WeaponInfo* weap, CharacterState movement, bool force=false);
    void refreshIdleAnims(const WeaponInfo* weap, CharacterState idle, bool force=false);

    void clearAnimQueue();

    bool updateWeaponState();
    bool updateCreatureState();
    void updateIdleStormState(bool inwater);

    void updateAnimQueue();

    void updateHeadTracking(float duration);

    void updateMagicEffects();

    void playDeath(float startpoint, CharacterState death);
    CharacterState chooseRandomDeathState() const;
    void playRandomDeath(float startpoint = 0.0f);

    /// choose a random animation group with \a prefix and numeric suffix
    /// @param num if non-NULL, the chosen animation number will be written here
    std::string chooseRandomGroup (const std::string& prefix, int* num = NULL) const;

    bool updateCarriedLeftVisible(WeaponType weaptype) const;

public:
    CharacterController(const MWWorld::Ptr &ptr, MWRender::Animation *anim);
    virtual ~CharacterController();

    virtual void handleTextKey(const std::string &groupname, const std::multimap<float, std::string>::const_iterator &key,
                       const std::multimap<float, std::string>& map);

    // Be careful when to call this, see comment in Actors
    void updateContinuousVfx();

    void updatePtr(const MWWorld::Ptr &ptr);

    void update(float duration);

	float frontCollisionDistance(float raylength, float zoffset);

	void recenterCameraRoll(float duration);

	bool checkForObstruction(float z, float distance, bool above = false);

	//MWPhysics::PhysicsSystem::RayResult getRayResult(float z, float distance, RayDirection dir, osg::Vec3f position);

	MWPhysics::PhysicsSystem::RayResult getRayResult(float z, float distance, osg::Vec3f position, RayDirection = RayDirection::forward);

	//bool checkForObstruction(float z, float distance);

	bool checkCanWallJump();

	ClimbData checkCanClimb();

	bool checkActions();

	/*bool updateClimb(float duration);

	bool startClimb(float z, float forward, osg::Vec3f direction);*/

	/*bool wallJump();

	bool updateWallJump(float duration);*/

    void persistAnimationState();
    void unpersistAnimationState();

    bool playGroup(const std::string &groupname, int mode, int count, bool persist=false);
    void skipAnim();
    bool isAnimPlaying(const std::string &groupName);

    enum KillResult
    {
        Result_DeathAnimStarted,
        Result_DeathAnimPlaying,
        Result_DeathAnimJustFinished,
        Result_DeathAnimFinished
    };
    KillResult kill();

    void resurrect();
    bool isDead() const
    { return mDeathState != CharState_None; }

    void forceStateUpdate();
    
    bool isAttackPrepairing() const;
    bool isReadyToBlock() const;
    bool isKnockedDown() const;
    bool isKnockedOut() const;
    bool isRecovery() const;
    bool isSneaking() const;
    bool isRunning() const;
    bool isTurning() const;
    bool isAttackingOrSpell() const;
	bool isClimbing() const;

	/*bool isWallJumping() const;*/
	
    void setAttackingOrSpell(bool attackingOrSpell);
    void setAIAttackType(const std::string& attackType);
    static void setAttackTypeRandomly(std::string& attackType);

    bool readyToPrepareAttack() const;
    bool readyToStartAttack() const;

    float getAttackStrength() const;

    /// @see Animation::setActive
    void setActive(bool active);

    /// Make this character turn its head towards \a target. To turn off head tracking, pass an empty Ptr.
    void setHeadTrackTarget(const MWWorld::ConstPtr& target);

    void playSwishSound(float attackStrength);
};

    MWWorld::ContainerStoreIterator getActiveWeapon(CreatureStats &stats, MWWorld::InventoryStore &inv, WeaponType *weaptype);
}

#endif /* GAME_MWMECHANICS_CHARACTER_HPP */
