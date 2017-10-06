#include "distantdialogue.hpp"

#include <MyGUI_LanguageManager.h>
#include <MyGUI_Window.h>
#include <MyGUI_ProgressBar.h>

#include <components/widgets/list.hpp>
#include <components/translation/translation.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/dialoguemanager.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/containerstore.hpp"
#include "../mwworld/esmstore.hpp"

#include "../mwmechanics/creaturestats.hpp"
#include "../mwmechanics/actorutil.hpp"

#include "widgets.hpp"
#include "bookpage.hpp"
#include "textcolours.hpp"

#include "../mwmechanics/aicalledover.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
//#include "../mwmechanics/aiwander.hpp"

#include "journalbooks.hpp" // to_utf8_span
namespace MWGui
{
	DistantDialogueWindow::DistantDialogueWindow() 
		: WindowBase("openmw_distant_dialogue_window.layout")
		, mEnabled(false)
		, mGoodbye(false)
	{
		center();
		
		MyGUI::Button* byeButton;
		getWidget(byeButton, "ByeButton");
		byeButton->eventMouseButtonClick += MyGUI::newDelegate(this, &DistantDialogueWindow::onByeClicked);

		//mMainWidget->castType<MyGUI::Window>()->eventWindowChangeCoord += MyGUI::newDelegate(this, &DistantDialogueWindow::onWindowResize);

		MyGUI::Button* calloverButton;
		getWidget(calloverButton, "CallOverButton");
		calloverButton->eventMouseButtonClick += MyGUI::newDelegate(this, &DistantDialogueWindow::onCallOverClicked);

		MyGUI::Button* waveButton;
		getWidget(waveButton, "WaveButton");
		waveButton->eventMouseButtonClick += MyGUI::newDelegate(this, &DistantDialogueWindow::onWaveClicked);

		mMainWidget->castType<MyGUI::Window>()->eventWindowChangeCoord += MyGUI::newDelegate(this, &DistantDialogueWindow::onWindowResize);
	}

	void DistantDialogueWindow::startDistantDialogue(MWWorld::Ptr actor, std::string npcName)
	{
		mPtr = actor;
		setTitle(npcName);
		mDisposition = MWBase::Environment::get().getMechanicsManager()->getDerivedDisposition(mPtr);
		
	}

	void DistantDialogueWindow::exit()
	{
		MWBase::Environment::get().getWindowManager()->removeGuiMode(GM_DistantDialogue);
	}

	
	void DistantDialogueWindow::onByeClicked(MyGUI::Widget* _sender)
	{
		exit();
	}

	void DistantDialogueWindow::onCallOverClicked(MyGUI::Widget* _sender)
	{
		
		MWMechanics::AiSequence& seq = mPtr.getClass().getCreatureStats(mPtr).getAiSequence();
		seq.stack(MWMechanics::AiCalledOver("player"), mPtr);
		exit();

		//Again, make this code in  npc logic?
	}

	void DistantDialogueWindow::onWaveClicked(MyGUI::Widget* _sender)
	{

		//if (mDisposition > 50)
		//{
			MWBase::Environment::get().getWindowManager()->messageBox(mPtr.getClass().getName(mPtr) + " waves back.");
			MWMechanics::NpcStats& npcStats = mPtr.getClass().getNpcStats(mPtr);
			npcStats.setBaseDisposition(static_cast<int>(npcStats.getBaseDisposition() + 10));
		//}
		//else
		//	MWBase::Environment::get().getWindowManager()->messageBox(mPtr.getClass().getName(mPtr) + " ignores you.");

		//Maybe each NPC has their own calculations? Example, some will wave no matter what. Some will not wave because they are focused. This works for now MWX
			MWMechanics::AiSequence& seq = mPtr.getClass().getCreatureStats(mPtr).getAiSequence();
			seq.stack(MWMechanics::AiWave("player"), mPtr);
			

		

		exit();
	}

	void DistantDialogueWindow::onWindowResize(MyGUI::Window* _sender)
	{
		// if the window has only been moved, not resized, we don't need to update
		if (mCurrentWindowSize == _sender->getSize()) return;
		mCurrentWindowSize = _sender->getSize();
	}
	
	void DistantDialogueWindow::onReferenceUnavailable()
	{
		MWBase::Environment::get().getWindowManager()->removeGuiMode(GM_DistantDialogue);
	}


}

