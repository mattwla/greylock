#include "confronttrespasser.hpp"

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
#include "../mwbase/tasksmanager.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/travelnodesmanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwworld/timestamp.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwbase/statusmanager.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/aiactivate.hpp"
#include "../mwmechanics/pathgrid.hpp"
#include "../mwmechanics/aicalledover.hpp"
#include "journey.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>

namespace MWTasks
{
	ConfrontTrespasser::ConfrontTrespasser()
	{
		mStep = 0;
		mDone = false;
		mTalkedTo = false;
	}

	ConfrontTrespasser::ConfrontTrespasser(MWTasks::Task* lifetask, MWWorld::Ptr target)
	{
		mLifeTask = lifetask;
		mNpcId = mLifeTask->mNpcId;
		mNpcPtr = mLifeTask->mNpcPtr;
		mStep = 0;
		mDone = false;
		mTarget = target;
		MWBase::Environment::get().getStatusManager()->giveStatus(mNpcPtr, MWBase::Fighting);
		MWBase::Environment::get().getStatusManager()->giveStatus(mNpcPtr, MWBase::ConfrontingTrespasser);
		std::cout << "made confronttrespasser task" << std::endl;
		mTalkedTo = false;
		mChasing = false;
	}

	ConfrontTrespasser::~ConfrontTrespasser()
	{

		if (mSubTask)
		{
			delete mSubTask;
		}
		
		MWBase::Environment::get().getStatusManager()->removeStatus(mNpcPtr, MWBase::ConfrontingTrespasser);
		MWBase::Environment::get().getStatusManager()->removeStatus(mNpcPtr, MWBase::Fighting);

	}

	MWWorld::Ptr ConfrontTrespasser::update()
	{

		//Check if target is within reach, if so either start fight or continue fight
		//If not within reach, either start journey or continue journey

		if (mStep == 0)
		{
			if (MWBase::Environment::get().getLifeManager()->inActiveRange(mNpcPtr))
			{
				if (mSubTask)
				{
					mNpcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
					delete mSubTask;
					mSubTask = 0;
				}
				MWMechanics::AiSequence& seq = mNpcPtr.getClass().getCreatureStats(mNpcPtr).getAiSequence();
				//We are near player, time to chat
				if (seq.getTypeId() != MWMechanics::AiPackage::TypeIdCombat) //mwx fix me this is hacky combat should be integrated with status and reactions system.
				{
					
					if (seq.getTypeId() != MWMechanics::AiPackage::TypeIdCalledOver && seq.getTypeId() != MWMechanics::AiPackage::TypeIdCombat && !mChasing)
					{
						seq.stack(MWMechanics::AiCalledOver(mTarget.getCellRef().getRefId()), mNpcPtr);
						std::cout << "Stacked calledover seq" << std::endl;
						mChasing = true;
					}

					if ((mNpcPtr.getRefData().getPosition().asVec3() - mTarget.getRefData().getPosition().asVec3()).length2()
						< 500 * 500
						&& MWBase::Environment::get().getWorld()->getLOS(mNpcPtr, mTarget))
					{
						std::cout << "STOP!" << std::endl;
						if (!mTalkedTo)
						{
							mTalkedTo = true;
							std::cout << "activating plater" << std::endl;
							//MWBase::Environment::get().getWorld()->activate(mTarget, mNpcPtr);
							MWBase::Environment::get().getWindowManager()->pushGuiMode(MWGui::GM_Dialogue, mNpcPtr);
							//MWBase::Environment::get().getWindowManager()->gui
						}
					/*	else
						{
							MWBase::Environment::get().getMechanicsManager()->startCombat(mNpcPtr, mTarget);
						}*/

					}
				}

			}
			else if (!mSubTask)
			{
				//mwx fix me horrid place for this code.
				//person went out of range
				unsigned int itx = 0;
				//while (itx < MWBase::Environment::get().getLifeManager()->mLifeList.size())
					mDone = true;
					/*{
					if (MWBase::Environment::get().getLifeManager()->mLifeList[itx]->mPtr == mNpcPtr)
					{
						delete MWBase::Environment::get().getLifeManager()->mLifeList[itx]->mSubTask;
						MWBase::Environment::get().getLifeManager()->mLifeList[itx]->mSubTask = 0;
						MWBase::Environment::get().getLifeManager()->mLifeList[itx]->mCurrentTask->resume();

					}
					itx += 1;
				}
				*/
				/*std::cout << "making a journey to pursue trespasser" << std::endl;
				mSubTask = new MWTasks::Journey(mTarget.getCellRef().getRefId(), mLifeTask);*/
			}
			else
			{
				mNpcPtr = mSubTask->update();
				if (mSubTask->mDone)
				{
					std::cout << "tresspass pursuit journey done" << std::endl;
					delete mSubTask;
					mSubTask = 0;
				}
			}

			mLifeTask->mNpcPtr = mNpcPtr;
			return mNpcPtr;
		}
	}

	int ConfrontTrespasser::getTypeId() const
	{
		return TypeIDConfrontTrespasser;
	}



}

