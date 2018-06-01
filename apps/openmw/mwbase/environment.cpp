#include "environment.hpp"

#include <cassert>

#include <OpenThreads/Thread>

#include "world.hpp"
#include "scriptmanager.hpp"
#include "dialoguemanager.hpp"
#include "journal.hpp"
#include "soundmanager.hpp"
#include "mechanicsmanager.hpp"
#include "inputmanager.hpp"
#include "windowmanager.hpp"
#include "statemanager.hpp"
#include "aischedulemanager.hpp"
#include "tasksmanager.hpp"
#include "travelnodesmanager.hpp"
#include "awarenessreactionsmanager.hpp"
#include "smartentitiesmanager.hpp"
#include "statusmanager.hpp"
#include "lifemanager.hpp"

MWBase::Environment *MWBase::Environment::sThis = 0;

MWBase::Environment::Environment()
: mWorld (0), mSoundManager (0), mScriptManager (0), mWindowManager (0),
  mMechanicsManager (0),  mDialogueManager (0), mJournal (0), mInputManager (0), mStateManager (0),
  mFrameDuration (0), mFrameRateLimit(0.f)
{
    assert (!sThis);
    sThis = this;
}

MWBase::Environment::~Environment()
{
    cleanup();
    sThis = 0;
}

void MWBase::Environment::setWorld (World *world)
{
    mWorld = world;
}

void MWBase::Environment::setSoundManager (SoundManager *soundManager)
{
    mSoundManager = soundManager;
}

void MWBase::Environment::setScriptManager (ScriptManager *scriptManager)
{
    mScriptManager = scriptManager;
}

void MWBase::Environment::setWindowManager (WindowManager *windowManager)
{
    mWindowManager = windowManager;
}

void MWBase::Environment::setMechanicsManager (MechanicsManager *mechanicsManager)
{
    mMechanicsManager = mechanicsManager;
}

void MWBase::Environment::setTasksManager(TasksManager *tasksManager)
{
	mTasksManager = tasksManager;
}

void MWBase::Environment::setTravelNodesManager(TravelNodesManager *travelNodesManager)
{
	mTravelNodesManager = travelNodesManager;
}

void MWBase::Environment::setLifeManager(LifeManager *lifeManager)
{
	mLifeManager = lifeManager;
}
void MWBase::Environment::setAIScheduleManager(AIScheduleManager *AIScheduleManager)
{
	mAIScheduleManager = AIScheduleManager;
}

void MWBase::Environment::setAwarenessReactionsManager(AwarenessReactionsManager * awarenessReactionsManager)
{
	mAwarenessReactionsManager = awarenessReactionsManager;
}

void MWBase::Environment::setSmartEntitiesManager(SmartEntitiesManager * smartEntitiesManager)
{
	mSmartEntitiesManager = smartEntitiesManager;
}

void MWBase::Environment::setStatusManager(StatusManager * statusManager)
{
	mStatusManager = statusManager;
}


void MWBase::Environment::setDialogueManager (DialogueManager *dialogueManager)
{
    mDialogueManager = dialogueManager;
}

void MWBase::Environment::setJournal (Journal *journal)
{
    mJournal = journal;
}

void MWBase::Environment::setInputManager (InputManager *inputManager)
{
    mInputManager = inputManager;
}

void MWBase::Environment::setStateManager (StateManager *stateManager)
{
    mStateManager = stateManager;
}

void MWBase::Environment::setFrameDuration (float duration)
{
    mFrameDuration = duration;
}

void MWBase::Environment::setFrameRateLimit(float limit)
{
    mFrameRateLimit = limit;
}

float MWBase::Environment::getFrameRateLimit() const
{
    return mFrameRateLimit;
}

void MWBase::Environment::limitFrameRate(double dt) const
{
    if (mFrameRateLimit > 0.f)
    {
        double thisFrameTime = dt;
        double minFrameTime = 1.0 / static_cast<double>(mFrameRateLimit);
        if (thisFrameTime < minFrameTime)
        {
            OpenThreads::Thread::microSleep(1000*1000*(minFrameTime-thisFrameTime));
        }
    }
}

MWBase::World *MWBase::Environment::getWorld() const
{
    assert (mWorld);
    return mWorld;
}

MWBase::SoundManager *MWBase::Environment::getSoundManager() const
{
    assert (mSoundManager);
    return mSoundManager;
}

MWBase::ScriptManager *MWBase::Environment::getScriptManager() const
{
    assert (mScriptManager);
    return mScriptManager;
}

MWBase::WindowManager *MWBase::Environment::getWindowManager() const
{
    assert (mWindowManager);
    return mWindowManager;
}

MWBase::MechanicsManager *MWBase::Environment::getMechanicsManager() const
{
    assert (mMechanicsManager);
    return mMechanicsManager;
}

MWBase::AIScheduleManager *MWBase::Environment::getAIScheduleManager() const
{
	assert(mAIScheduleManager);
	return mAIScheduleManager;
}

MWBase::TasksManager *MWBase::Environment::getTasksManager() const
{
	assert(mTasksManager);
	return mTasksManager;
}

MWBase::AwarenessReactionsManager *MWBase::Environment::getAwarenessReactionsManager() const
{
	assert(mAwarenessReactionsManager);
	return(mAwarenessReactionsManager);
}

MWBase::SmartEntitiesManager *MWBase::Environment::getSmartEntitiesManager() const
{
	assert(mSmartEntitiesManager);
	return(mSmartEntitiesManager);
}

//MWBase::StatusManager * MWBase::Environment::getStatusManager() const
//{
//	assert(mStatusManager);
//	return(mStatusManager);
//}

MWBase::TravelNodesManager *MWBase::Environment::getTravelNodesManager() const
{
	assert(mTravelNodesManager);
	return mTravelNodesManager;
}

MWBase::DialogueManager *MWBase::Environment::getDialogueManager() const
{
    assert (mDialogueManager);
    return mDialogueManager;
}

MWBase::LifeManager *MWBase::Environment::getLifeManager() const
{
	assert(mLifeManager);
	return mLifeManager;
}

MWBase::Journal *MWBase::Environment::getJournal() const
{
    assert (mJournal);
    return mJournal;
}

MWBase::InputManager *MWBase::Environment::getInputManager() const
{
    assert (mInputManager);
    return mInputManager;
}

MWBase::StateManager *MWBase::Environment::getStateManager() const
{
    assert (mStateManager);
    return mStateManager;
}

float MWBase::Environment::getFrameDuration() const
{
    return mFrameDuration;
}

void MWBase::Environment::cleanup()
{
    delete mMechanicsManager;
    mMechanicsManager = 0;

    delete mDialogueManager;
    mDialogueManager = 0;

    delete mJournal;
    mJournal = 0;

    delete mScriptManager;
    mScriptManager = 0;

    delete mWindowManager;
    mWindowManager = 0;

    delete mWorld;
    mWorld = 0;

    delete mSoundManager;
    mSoundManager = 0;

    delete mInputManager;
    mInputManager = 0;

    delete mStateManager;
    mStateManager = 0;

	delete mAIScheduleManager;
	mAIScheduleManager = 0;
	
	delete mAwarenessReactionsManager;
	mAwarenessReactionsManager = 0;

	delete mStatusManager;
	mStatusManager = 0;

	delete mSmartEntitiesManager;
	mSmartEntitiesManager = 0;

	delete mTasksManager;
	mTasksManager = 0;

	delete mTravelNodesManager;
	mTravelNodesManager = 0;
}

const MWBase::Environment& MWBase::Environment::get()
{
    assert (sThis);
    return *sThis;
}
