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

#include "../mwmechanics/aifollow.hpp"

#include "journalbooks.hpp" // to_utf8_span
namespace MWGui
{
	DistantDialogueWindow::DistantDialogueWindow() 
		: WindowBase("openmw_distant_dialogue_window.layout")
		, mEnabled(false)
		, mGoodbye(false)
	{
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
		//mGoodbye = false;
		//mEnabled = true;
		//bool sameActor = (mPtr == actor);
		//mPtr = actor;
		//mTopicsList->setEnabled(true);
		//setTitle(npcName);

		//clearChoices();

		//mTopicsList->clear();

		/*if (resetHistory || !sameActor)
		{
			for (std::vector<DialogueText*>::iterator it = mHistoryContents.begin(); it != mHistoryContents.end(); ++it)
				delete (*it);
			mHistoryContents.clear();
		}*/

		/*for (std::vector<Link*>::iterator it = mLinks.begin(); it != mLinks.end(); ++it)
			delete (*it);
		mLinks.clear();*/

		//updateOptions();

		//restock();
	}

	void DistantDialogueWindow::exit()
	{
		MWBase::Environment::get().getWindowManager()->removeGuiMode(GM_DistantDialogue);
			MWBase::Environment::get().getDialogueManager()->goodbyeSelected();
			//mTopicsList->scrollToTop();
		
	}

	
	void DistantDialogueWindow::onByeClicked(MyGUI::Widget* _sender)
	{
		exit();
	}

	void DistantDialogueWindow::onCallOverClicked(MyGUI::Widget* _sender)
	{
		
		MWMechanics::AiSequence& seq = mPtr.getClass().getCreatureStats(mPtr).getAiSequence();



		
		seq.stack(MWMechanics::AiFollow("player"), mPtr);

		
		
		exit();
	}

	void DistantDialogueWindow::onWaveClicked(MyGUI::Widget* _sender)
	{

	



		exit();
	}

	void DistantDialogueWindow::onWindowResize(MyGUI::Window* _sender)
	{
		// if the window has only been moved, not resized, we don't need to update
		if (mCurrentWindowSize == _sender->getSize()) return;

		//mTopicsList->adjustSize();
		//updateHistory();
		mCurrentWindowSize = _sender->getSize();
	}
	void DistantDialogueWindow::onReferenceUnavailable()
	{
		MWBase::Environment::get().getWindowManager()->removeGuiMode(GM_DistantDialogue);
	}


}

