#include "dialogue.hpp"

#include <MyGUI_LanguageManager.h>
#include <MyGUI_Window.h>
#include <MyGUI_ProgressBar.h>
#include <MyGUI_FontManager.h>
#include <MyGUI_ScrollBar.h>
#include <MyGUI_Button.h>


#include <components/widgets/list.hpp>
#include <components/translation/translation.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/inputmanager.hpp"
#include <components/resource/resourcesystem.hpp>
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/statusmanager.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/containerstore.hpp"
#include "../mwworld/esmstore.hpp"

#include "../mwmechanics/creaturestats.hpp"
#include "../mwmechanics/actorutil.hpp"

#include "bookpage.hpp"
#include "textcolours.hpp"

#include "journalbooks.hpp" // to_utf8_span

namespace MWGui
{

    class ResponseCallback : public MWBase::DialogueManager::ResponseCallback
    {
    public:
        ResponseCallback(DialogueWindow* win, bool needMargin=true)
            : mWindow(win)
            , mNeedMargin(needMargin)
        {

        }

        void addResponse(const std::string& title, const std::string& text)
        {
            mWindow->addResponse(title, text, mNeedMargin);
        }

        void updateTopics()
        {
            mWindow->updateTopics();
        }

    private:
        DialogueWindow* mWindow;
        bool mNeedMargin;
    };

    PersuasionDialog::PersuasionDialog(ResponseCallback* callback)
        : WindowModal("openmw_persuasion_dialog.layout")
        , mCallback(callback)
    {
        getWidget(mCancelButton, "CancelButton");
        getWidget(mAdmireButton, "AdmireButton");
        getWidget(mIntimidateButton, "IntimidateButton");
        getWidget(mTauntButton, "TauntButton");
        getWidget(mBribe10Button, "Bribe10Button");
        getWidget(mBribe100Button, "Bribe100Button");
        getWidget(mBribe1000Button, "Bribe1000Button");
        getWidget(mGoldLabel, "GoldLabel");

        mCancelButton->eventMouseButtonClick += MyGUI::newDelegate(this, &PersuasionDialog::onCancel);
        mAdmireButton->eventMouseButtonClick += MyGUI::newDelegate(this, &PersuasionDialog::onPersuade);
        mIntimidateButton->eventMouseButtonClick += MyGUI::newDelegate(this, &PersuasionDialog::onPersuade);
        mTauntButton->eventMouseButtonClick += MyGUI::newDelegate(this, &PersuasionDialog::onPersuade);
        mBribe10Button->eventMouseButtonClick += MyGUI::newDelegate(this, &PersuasionDialog::onPersuade);
        mBribe100Button->eventMouseButtonClick += MyGUI::newDelegate(this, &PersuasionDialog::onPersuade);
        mBribe1000Button->eventMouseButtonClick += MyGUI::newDelegate(this, &PersuasionDialog::onPersuade);
    }

    void PersuasionDialog::onCancel(MyGUI::Widget *sender)
    {
        setVisible(false);
    }

    void PersuasionDialog::onPersuade(MyGUI::Widget *sender)
    {
        MWBase::MechanicsManager::PersuasionType type;
        if (sender == mAdmireButton) type = MWBase::MechanicsManager::PT_Admire;
        else if (sender == mIntimidateButton) type = MWBase::MechanicsManager::PT_Intimidate;
        else if (sender == mTauntButton) type = MWBase::MechanicsManager::PT_Taunt;
        else if (sender == mBribe10Button)
            type = MWBase::MechanicsManager::PT_Bribe10;
        else if (sender == mBribe100Button)
            type = MWBase::MechanicsManager::PT_Bribe100;
        else /*if (sender == mBribe1000Button)*/
            type = MWBase::MechanicsManager::PT_Bribe1000;


        //MWBase::Environment::get().getDialogueManager()->persuade(type, mCallback.get());

        mCallback->updateTopics();


        setVisible(false);
    }

    void PersuasionDialog::onOpen()
    {
        center();

        MWWorld::Ptr player = MWMechanics::getPlayer();
        int playerGold = player.getClass().getContainerStore(player).count(MWWorld::ContainerStore::sGoldId);

        mBribe10Button->setEnabled (playerGold >= 10);
        mBribe100Button->setEnabled (playerGold >= 100);
        mBribe1000Button->setEnabled (playerGold >= 1000);

        mGoldLabel->setCaptionWithReplacing("#{sGold}: " + MyGUI::utility::toString(playerGold));
        WindowModal::onOpen();
    }

    MyGUI::Widget* PersuasionDialog::getDefaultKeyFocus()
    {
        return mAdmireButton;
    }

    // --------------------------------------------------------------------------------------------------

    Response::Response(const std::string &text, const std::string &title, bool needMargin)
        : mTitle(title), mNeedMargin(needMargin)
    {
        mText = text;
		mType = response;
    }

	std::string Response::parsePortraitTags(std::string text) const
	{
		//Takes in text chunk, scans for emotional tag at start and if found sends request to setPortrait to alter image, also strips tag from text and returns the clean text.
		
		MWWorld::Ptr ptr = MWBase::Environment::get().getWindowManager()->getDialogueWindow()->getDialogueHost();
		std::string name = ptr.getCellRef().getRefId();
		bool speaker_changed = false;
		
		
		
		char chunk = text.at(0);
		if (chunk == *"=")
		{
			text = text.substr(1);
			std::size_t pos = text.find("=");
			name = text.substr(0, pos);
			text = text.substr(pos+1);
			speaker_changed = true;
		}
		//If we want to change characters, type in their id between 2 equal marks such as |=arx=a/I AM ANGRY
		chunk = text.at(0);
		
		
		
		/*if (chunk == *"a")
		{
			MWBase::Environment::get().getWindowManager()->getDialogueWindow()->setPortraitImage(name, "A");
			return text.substr(2);
		}*/
		/*else*/
			MWBase::Environment::get().getWindowManager()->getDialogueWindow()->setPortraitImage(name);
		
		//if (speaker_changed)
			return name + ": " + text;
		//else
		//	return text;

	}

    void Response::write(BookTypesetter::Ptr typesetter, KeywordSearchT* keywordSearch, std::map<std::string, Link*>& topicLinks) const
    {
        typesetter->sectionBreak(mNeedMargin ? 9 : 0);

        if (false) //(mTitle != "")
        {
            const MyGUI::Colour& headerColour = MWBase::Environment::get().getWindowManager()->getTextColours().header;
            BookTypesetter::Style* title = typesetter->createStyle("", headerColour);
            typesetter->write(title, to_utf8_span(mTitle.c_str()));
            typesetter->sectionBreak();
        }

        typedef std::pair<size_t, size_t> Range;
        std::map<Range, intptr_t> hyperLinks;

        // We need this copy for when @# hyperlinks are replaced
		std::string text;
		//split text is generated in the updatehistory function, splits the dialogue by a dividing marker and stores it as a member vector variable MWX
		//mCurrent_chunk iteration done by next chunk action, which is called by input manager when player clicks.
		if (mSplitText.size() > 1)
			text = mSplitText[mCurrent_chunk];
		else
			text = mText;

		text = parsePortraitTags(text);
        size_t pos_end;
        for(;;)
        {
            size_t pos_begin = text.find('@');
            if (pos_begin != std::string::npos)
                pos_end = text.find('#', pos_begin);

            if (pos_begin != std::string::npos && pos_end != std::string::npos)
            {
                std::string link = text.substr(pos_begin + 1, pos_end - pos_begin - 1);
                const char specialPseudoAsteriskCharacter = 127;
                std::replace(link.begin(), link.end(), specialPseudoAsteriskCharacter, '*');
                std::string topicName = MWBase::Environment::get().getWindowManager()->
                        getTranslationDataStorage().topicStandardForm(link);

                std::string displayName = link;
                while (displayName[displayName.size()-1] == '*')
                    displayName.erase(displayName.size()-1, 1);

                text.replace(pos_begin, pos_end+1-pos_begin, displayName);

                if (topicLinks.find(Misc::StringUtils::lowerCase(topicName)) != topicLinks.end())
                    hyperLinks[std::make_pair(pos_begin, pos_begin+displayName.size())] = intptr_t(topicLinks[Misc::StringUtils::lowerCase(topicName)]);
            }
            else
                break;
        }

        typesetter->addContent(to_utf8_span(text.c_str()));

        if (hyperLinks.size() && MWBase::Environment::get().getWindowManager()->getTranslationDataStorage().hasTranslation())
        {
            const TextColours& textColours = MWBase::Environment::get().getWindowManager()->getTextColours();

            BookTypesetter::Style* style = typesetter->createStyle("", textColours.normal);
            size_t formatted = 0; // points to the first character that is not laid out yet
            for (std::map<Range, intptr_t>::iterator it = hyperLinks.begin(); it != hyperLinks.end(); ++it)
            {
                intptr_t topicId = it->second;
                BookTypesetter::Style* hotStyle = typesetter->createHotStyle (style, textColours.link,
                                                                              textColours.linkOver, textColours.linkPressed,
                                                                              topicId);
                if (formatted < it->first.first)
                    typesetter->write(style, formatted, it->first.first);
                typesetter->write(hotStyle, it->first.first, it->first.second);
                formatted = it->first.second;
            }
            if (formatted < text.size())
                typesetter->write(style, formatted, text.size());
        }
        else
        {
            std::vector<KeywordSearchT::Match> matches;
            //keywordSearch->highlightKeywords(text.begin(), text.end(), matches);

            std::string::const_iterator i = text.begin ();
            for (std::vector<KeywordSearchT::Match>::iterator it = matches.begin(); it != matches.end(); ++it)
            {
                KeywordSearchT::Match match = *it;
                if (i != match.mBeg)
                    addTopicLink (typesetter, 0, i - text.begin (), match.mBeg - text.begin ());

                addTopicLink (typesetter, match.mValue, match.mBeg - text.begin (), match.mEnd - text.begin ());

                i = match.mEnd;
            }
            if (i != text.end ())
                addTopicLink (typesetter, 0, i - text.begin (), text.size ());
        }


    }

    void Response::addTopicLink(BookTypesetter::Ptr typesetter, intptr_t topicId, size_t begin, size_t end) const
    {
        const TextColours& textColours = MWBase::Environment::get().getWindowManager()->getTextColours();

        BookTypesetter::Style* style = typesetter->createStyle("", textColours.normal);


        if (topicId)
            style = typesetter->createHotStyle (style, textColours.link, textColours.linkOver, textColours.linkPressed, topicId);
       
		typesetter->write (style, begin, end);
    }

    Message::Message(const std::string& text)
    {
        mText = text;
		mType = message;
    }

    void Message::write(BookTypesetter::Ptr typesetter, KeywordSearchT* keywordSearch, std::map<std::string, Link*>& topicLinks) const
    {
        const MyGUI::Colour& textColour = MWBase::Environment::get().getWindowManager()->getTextColours().notify;
        BookTypesetter::Style* title = typesetter->createStyle("", textColour);
        typesetter->sectionBreak(9);
        typesetter->write(title, to_utf8_span(mText.c_str()));
    }

    // --------------------------------------------------------------------------------------------------

    void Choice::activated()
    {
        MWBase::Environment::get().getWindowManager()->playSound("Menu Click");
        eventChoiceActivated(mChoiceId);
    }

    void Topic::activated()
    {
        MWBase::Environment::get().getWindowManager()->playSound("Menu Click");
        eventTopicActivated(mTopicId);
    }

	void NextChunk::activated() //MWX, when a player is being presented with sequential chunked dialogue, input manager takes over the mouse control, runs this function when mouse is pressed.
	{

		MWBase::Environment::get().getWindowManager()->playSound("Menu Click"); 
		MWBase::Environment::get().getDialogueManager()->nextChunkSelected();
	}

	void Go::activated() //MWX, when a player is being presented with sequential chunked dialogue, input manager takes over the mouse control, runs this function when mouse is pressed.
	{

		MWBase::Environment::get().getWindowManager()->playSound("Menu Click");
		MWBase::Environment::get().getDialogueManager()->nextChunkSelected();
	}



    void Goodbye::activated()
    {
        MWBase::Environment::get().getWindowManager()->playSound("Menu Click");
        eventActivated();
    }



    // --------------------------------------------------------------------------------------------------

    DialogueWindow::DialogueWindow()
        : WindowBase("openmw_dialogue_window.layout")
        , mIsCompanion(false)
        , mGoodbye(false)

		, mDialogueHost()
		//, mResourceSystem(Resource::ResourceSystem)
        //, mPersuasionDialog()

        , mPersuasionDialog(new ResponseCallback(this))
        , mCallback(new ResponseCallback(this))
        , mGreetingCallback(new ResponseCallback(this, false))

    {
        // Centre dialog
        center();


        //mPersuasionDialog.setVisible(false);
        //mPersuasionDialog.eventPersuadeMsg += MyGUI::newDelegate(this, &DialogueWindow::onPersuadeResult);

        mPersuasionDialog.setVisible(false);


        //History view
        getWidget(mHistory, "History");

		//A box around history, used for graphical effect.
		getWidget(mHistoryBox, "HistoryBox");

        //Topics list
		getWidget(mTopicsBox, "TopicsBox");
		getWidget(mTopicsList, "TopicsList");
		

		
		//The player portrait, currently not in use.
		getWidget(mPlayerPortrait, "PlayerPortrait");

		getWidget(mPortraitBox, "PortraitBox");
		
		//The NPC portrait
		getWidget(mNpcPortrait, "NPCPortrait");

		//mTopicsList->eventItemSelected += MyGUI::newDelegate(this, &DialogueWindow::onSelectTopic);

        mTopicsList->eventItemSelected += MyGUI::newDelegate(this, &DialogueWindow::onSelectListItem);


        getWidget(mGoodbyeButton, "ByeButton");
        mGoodbyeButton->eventMouseButtonClick += MyGUI::newDelegate(this, &DialogueWindow::onByeClicked);

        //getWidget(mDispositionBar, "Disposition");
        //getWidget(mDispositionText,"DispositionText");
		//Disposition not shown in Greylock for immersion sake MWX
        getWidget(mScrollBar, "VScroll");

        mScrollBar->eventScrollChangePosition += MyGUI::newDelegate(this, &DialogueWindow::onScrollbarMoved);
        mHistory->eventMouseWheel += MyGUI::newDelegate(this, &DialogueWindow::onMouseWheel);

        BookPage::ClickCallback callback = std::bind (&DialogueWindow::notifyLinkClicked, this, std::placeholders::_1);
        mHistory->adviseLinkClicked(callback);

        mMainWidget->castType<MyGUI::Window>()->eventWindowChangeCoord += MyGUI::newDelegate(this, &DialogueWindow::onWindowResize);
		mNpcPortrait->setImageTexture("textures\\portraits\\unknownportrait.dds");
		//mPlayerPortrait->setImageTexture("textures\\face.dds");
		mNpcPortrait->setVisible(false);
		mNpcPortrait->setPosition(0, 0);
		mPlayerPortrait->setVisible(false);
		mTopicsList->setVisible(true);
		mTopicsBox->setVisible(false);
		mPortraitBox->setVisible(false);
		//mNpcPortraitBox->setVisible(false);

		
		
		//Player portrait not used thus hidden, NPC portrait is used. Image textures are placeholders for now. MWX
		
	}

    DialogueWindow::~DialogueWindow()
    {
        deleteLater();
        for (Link* link : mLinks)
            delete link;
        for (auto link : mTopicLinks)
            delete link.second;
        for (auto history : mHistoryContents)
            delete history;
    }

    void DialogueWindow::onTradeComplete()
    {
        addResponse("", MyGUI::LanguageManager::getInstance().replaceTags("#{sBarterDialog5}"));
    }

    bool DialogueWindow::exit()
    {
		//mwx goodbye hack fix
     /*   if ((MWBase::Environment::get().getDialogueManager()->isInChoice()) || MWBase::Environment::get().getInputManager()->getInChunkMode())
        {
            return false;
			
        }
        else
        {*/
            resetReference();
            MWBase::Environment::get().getDialogueManager()->goodbyeSelected();
            mTopicsList->scrollToTop();
            return true;
      /*  }*/
    }

	MWWorld::Ptr DialogueWindow::getDialogueHost() const
	{
		return mDialogueHost;

		
		// TODO: insert return statement here
	}

    void DialogueWindow::onWindowResize(MyGUI::Window* _sender)
    {
        // if the window has only been moved, not resized, we don't need to update
        if (mCurrentWindowSize == _sender->getSize()) return;

        mTopicsList->adjustSize();
		adjustPortraitSize();
        updateHistory();
        mCurrentWindowSize = _sender->getSize();
    }

	void DialogueWindow::adjustPortraitSize()
	{
		MyGUI::IntSize screenSize = mMainWidget->getSize();

		
		int portraitWidth = screenSize.width / 3.5;
		int portraitHeight = portraitWidth * 1.333333333;

		mPortraitBox->setCoord(screenSize.width/2-portraitWidth/2, 0, portraitWidth, portraitHeight);
		mNpcPortrait->setCoord( portraitWidth*.05, portraitHeight*.05, portraitWidth*.9, portraitHeight*.9);
		//3:4 ratio for portrait width to height
	
		//mPlayerPortrait->setCoord(0, 0, portraitWidth, portraitHeight);
		//Player portrait unused for now. MWX

		//mNpcPortrait->setCoord((screenSize.width/3) * 2, screenSize.height/2, portraitWidth, portraitHeight);
		//mNpcPortrait->setCoord(-50, screenSize.height/2, portraitWidth, portraitHeight);
		//mPortraitBox->setSize(portraitWidth, portraitHeight);
		//mPortraitBox->setAlign(MyGUI::Align::Enum::Center);
		//MyGUI::Align Center;
		/*mNpcPortrait->setRealSize(.9, .9);
		MyGUI::IntSize picSize = mNpcPortrait->getSize();
		int picWidth = picSize.width;
		int picHeight = picSize.height;*/
		//mNpcPortrait->setCoord(((screenSize.width/2) - picWidth/2), screenSize.height/2, picWidth, picHeight);
		
		
	}

	

    void DialogueWindow::onMouseWheel(MyGUI::Widget* _sender, int _rel)
    {
        if (!mScrollBar->getVisible())
            return;
        mScrollBar->setScrollPosition(std::min(static_cast<int>(mScrollBar->getScrollRange()-1),
                                               std::max(0, static_cast<int>(mScrollBar->getScrollPosition() - _rel*0.3))));
        onScrollbarMoved(mScrollBar, mScrollBar->getScrollPosition());
    }

    void DialogueWindow::onByeClicked(MyGUI::Widget* _sender)
    {
        if (exit())
            MWBase::Environment::get().getWindowManager()->removeGuiMode(GM_Dialogue);
    }

    void DialogueWindow::onSelectListItem(const std::string& topic, int id)
    {
        if (mGoodbye ||  MWBase::Environment::get().getDialogueManager()->isInChoice())
            return;

        int separatorPos = 0;
        for (unsigned int i=0; i<mTopicsList->getItemCount(); ++i)
        {
            if (mTopicsList->getItemNameAt(i) == "")
                separatorPos = i;
        }

        if (id >= separatorPos)
        {
            onTopicActivated(topic);
            if (mGoodbyeButton->getEnabled())
                MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(mGoodbyeButton);
        }
        else
        {
            const MWWorld::Store<ESM::GameSetting> &gmst =
                MWBase::Environment::get().getWorld()->getStore().get<ESM::GameSetting>();

            if (topic == gmst.find("sPersuasion")->getString())
                mPersuasionDialog.setVisible(true);
            else if (topic == gmst.find("sCompanionShare")->getString())
                MWBase::Environment::get().getWindowManager()->pushGuiMode(GM_Companion, mPtr);
            else if (!MWBase::Environment::get().getDialogueManager()->checkServiceRefused(mCallback.get()))
            {
                if (topic == gmst.find("sBarter")->getString())
                    MWBase::Environment::get().getWindowManager()->pushGuiMode(GM_Barter, mPtr);
                else if (topic == gmst.find("sSpells")->getString())
                    MWBase::Environment::get().getWindowManager()->pushGuiMode(GM_SpellBuying, mPtr);
                else if (topic == gmst.find("sTravel")->getString())
                    MWBase::Environment::get().getWindowManager()->pushGuiMode(GM_Travel, mPtr);
                else if (topic == gmst.find("sSpellMakingMenuTitle")->getString())
                    MWBase::Environment::get().getWindowManager()->pushGuiMode(GM_SpellCreation, mPtr);
                else if (topic == gmst.find("sEnchanting")->getString())
                    MWBase::Environment::get().getWindowManager()->pushGuiMode(GM_Enchanting, mPtr);
                else if (topic == gmst.find("sServiceTrainingTitle")->getString())
                    MWBase::Environment::get().getWindowManager()->pushGuiMode(GM_Training, mPtr);
                else if (topic == gmst.find("sRepair")->getString())
                    MWBase::Environment::get().getWindowManager()->pushGuiMode(GM_MerchantRepair, mPtr);
            }
            else
                updateTopics();
        }
    }

    void DialogueWindow::setPtr(const MWWorld::Ptr& actor)
    {


		mMainWidget->setRealSize(1, 1);
		mHistoryBox->setRealSize(.75, .3);
		//Make sure we use our static almost full screen dialogue concept MWX
		center();
		adjustPortraitSize();
		//Make sure portraits are right size in case player changed resolution MWX
		mDialogueHost = actor;
		//Keep track of which NPC is hosting this dialogue
	
		
		mGoodbye = false;
        mEnabled = true;
        bool sameActor = (mPtr == actor);
        mPtr = actor;
        mTopicsList->setEnabled(true);
		mTopicsBox->setVisible(false);
        setTitle(""); //Empty title as dialogue screens will be used for multiple speakers now. MWX
		
		//clearChoices();

       /* MWBase::DialogueManager::Response response;
        if (!MWBase::Environment::get().getDialogueManager()->startDialogue(actor, response))*/
      
        if (!sameActor)
        {
            for (std::vector<DialogueText*>::iterator it = mHistoryContents.begin(); it != mHistoryContents.end(); ++it)
                delete (*it);
            mHistoryContents.clear();
            mKeywords.clear();
            mTopicsList->clear();
            for (std::vector<Link*>::iterator it = mLinks.begin(); it != mLinks.end(); ++it)
                mDeleteLater.push_back(*it); // Links are not deleted right away to prevent issues with event handlers
            mLinks.clear();
        }

        mPtr = actor;
        mGoodbye = false;
        mTopicsList->setEnabled(true);

        if (!MWBase::Environment::get().getDialogueManager()->startDialogue(actor, mGreetingCallback.get()))
        {
            // No greetings found. The dialogue window should not be shown.
            // If this is a companion, we must show the companion window directly (used by BM_bear_be_unique).
            MWBase::Environment::get().getWindowManager()->removeGuiMode(MWGui::GM_Dialogue);
            mPtr = MWWorld::Ptr();
            if (isCompanion(actor))
                MWBase::Environment::get().getWindowManager()->pushGuiMode(MWGui::GM_Companion, actor);
            return;
        }
	

        MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(mGoodbyeButton);


        mGoodbye = false;
        //bool sameActor = (mPtr == actor);
        mPtr = actor;
        mTopicsList->setEnabled(true);
      


        setTitle(mPtr.getClass().getName(mPtr));

        updateTopics();
        updateTopicsPane(); // force update for new services

        updateDisposition();
        restock();
		/*addResponse(response.first, response.second, false);*/
		/*if (MWBase::Environment::get().getStatusManager()->hasStatus(mPtr, MWBase::ConfrontingTrespasser))
		{
			mChoices.clear();
			onTopicActivated("confront trespasser");
		}*/

    }


    void DialogueWindow::restock()
    {
        MWMechanics::CreatureStats &sellerStats = mPtr.getClass().getCreatureStats(mPtr);
        float delay = MWBase::Environment::get().getWorld()->getStore().get<ESM::GameSetting>().find("fBarterGoldResetDelay")->getFloat();

        // Gold is restocked every 24h
        if (MWBase::Environment::get().getWorld()->getTimeStamp() >= sellerStats.getLastRestockTime() + delay)
        {
            sellerStats.setGoldPool(mPtr.getClass().getBaseGold(mPtr));

            sellerStats.setLastRestockTime(MWBase::Environment::get().getWorld()->getTimeStamp());
        }
    }

    void DialogueWindow::deleteLater()
    {
        for (Link* link : mDeleteLater)
            delete link;
        mDeleteLater.clear();
    }

    void DialogueWindow::setKeywords(std::list<std::string> keyWords)
    {
        if (mKeywords == keyWords && isCompanion() == mIsCompanion)
            return;
        mIsCompanion = isCompanion();
        mKeywords = keyWords;

        updateTopicsPane();
    }

    void DialogueWindow::updateTopicsPane()
    {
        mTopicsList->clear();
        for (std::map<std::string, Link*>::iterator it = mTopicLinks.begin(); it != mTopicLinks.end(); ++it)
            mDeleteLater.push_back(it->second);
        mTopicLinks.clear();
        mKeywordSearch.clear();

        int services = mPtr.getClass().getServices(mPtr);

        bool travel = (mPtr.getTypeName() == typeid(ESM::NPC).name() && !mPtr.get<ESM::NPC>()->mBase->getTransport().empty())
                || (mPtr.getTypeName() == typeid(ESM::Creature).name() && !mPtr.get<ESM::Creature>()->mBase->getTransport().empty());

        const MWWorld::Store<ESM::GameSetting> &gmst =
            MWBase::Environment::get().getWorld()->getStore().get<ESM::GameSetting>();

        //if (mPtr.getTypeName() == typeid(ESM::NPC).name())
           // mTopicsList->addItem(gmst.find("sPersuasion")->getString());
			//MWX no more persuasion tab, that is not how we woo NPCs in Greylock (Woo with player action and player sharing experiences/values)

        if (services & ESM::NPC::AllItems)
            mTopicsList->addItem(gmst.find("sBarter")->getString());

        if (services & ESM::NPC::Spells)
            mTopicsList->addItem(gmst.find("sSpells")->getString());

        if (travel)
            mTopicsList->addItem(gmst.find("sTravel")->getString());

        if (services & ESM::NPC::Spellmaking)
            mTopicsList->addItem(gmst.find("sSpellmakingMenuTitle")->getString());

        if (services & ESM::NPC::Enchanting)
            mTopicsList->addItem(gmst.find("sEnchanting")->getString());

        if (services & ESM::NPC::Training)
            mTopicsList->addItem(gmst.find("sServiceTrainingTitle")->getString());

        if (services & ESM::NPC::Repair)
            mTopicsList->addItem(gmst.find("sRepair")->getString());

        if (isCompanion())
            mTopicsList->addItem(gmst.find("sCompanionShare")->getString());

        if (mTopicsList->getItemCount() > 0)
            mTopicsList->addSeparator();


        for(std::list<std::string>::iterator it = mKeywords.begin(); it != mKeywords.end(); ++it)
        {
            mTopicsList->addItem(*it);

            Topic* t = new Topic(*it);
            t->eventTopicActivated += MyGUI::newDelegate(this, &DialogueWindow::onTopicActivated);
            mTopicLinks[Misc::StringUtils::lowerCase(*it)] = t;

            mKeywordSearch.seed(Misc::StringUtils::lowerCase(*it), intptr_t(t));
        }
        mTopicsList->adjustSize();

        updateHistory();
    }
	
	std::vector<std::string> DialogueWindow::splitText(std::string text)
	{
		//This function takes a string (the response of NPC) and splits it by the pipe | character, returns a vector.
		
		std::vector<std::string> split;
		std::string delim = "|";
		auto start = 0U;
		auto end = text.find(delim);
		while (end != std::string::npos)
		{
			split.push_back(text.substr(start, end - start));
			start = end + delim.length();
			end = text.find(delim, start);
		}

		if (std::size(text.substr(start, end)) > 0) //Fix for accidental pipe usage at end of dialogue
			split.push_back(text.substr(start, end));

		return split;
		//Thanks to Moswald at stack overflow.
	}



	void DialogueWindow::updateHistory(bool scrollbar)
    {
		bool inChunk = true; 
		
		//We will use this to determine later behavior. are we in middle of dialogue flow or not?
		MWBase::Environment::get().getInputManager()->dialogueGoMode(false); //Assume we are not about to use GO work around unless we intentionally flip it on.
		if (!scrollbar && mScrollBar->getVisible())
        {
            mHistory->setSize(mHistory->getSize()+MyGUI::IntSize(mScrollBar->getWidth(),0));
            mScrollBar->setVisible(false);
        }
        if (scrollbar && !mScrollBar->getVisible())
        {
            mHistory->setSize(mHistory->getSize()-MyGUI::IntSize(mScrollBar->getWidth(),0));
            mScrollBar->setVisible(true);
        }

        BookTypesetter::Ptr typesetter = BookTypesetter::create (mHistory->getWidth(), std::numeric_limits<int>::max());

        
		if (mHistoryContents.size() > 1) //Different logic depending on if this is greeting or deeper in dialogue.
		{
			//for (std::vector<DialogueText*>::iterator it = mHistoryContents.begin()+(mHistoryContents.size()-1); it != mHistoryContents.end(); ++it)
			//{
				
			DialogueText * it = mHistoryContents.back();
			if (it->mType == message)
			{
				it = mHistoryContents[mHistoryContents.size() - 2];
			}
			it->mSplitText = splitText(it->mText); //take our mSplitText member variable, store the split dialogue in it.
			it->write(typesetter, &mKeywordSearch, mTopicLinks);
				
				if (it->mCurrent_chunk < it->mSplitText.size() - 1)
				{
					inChunk = true;
					mTopicsList->setVisible(false);
					MWBase::Environment::get().getInputManager()->dialogueChunkMode(true);
				}
				else {
					MWBase::Environment::get().getInputManager()->dialogueChunkMode(false);
					if (mHistoryContents.back()->mType == message) //do we have a message we need to show? MWX make sure it is not a choice MWX removed choice checking
						mHistoryContents.back()->write(typesetter, &mKeywordSearch, mTopicLinks);
					mTopicsList->setVisible(true);
					inChunk = false;
				}
			//	}
			//}
		}
		else
		{
			for (std::vector<DialogueText*>::iterator it = mHistoryContents.begin(); it != mHistoryContents.end(); ++it)
			{//(*it)->write(typesetter, &mKeywordSearch, mTopicLinks);
				(*it)->mSplitText = splitText((*it)->mText); //take our mSplitText member variable, store the split dialogue in it.
				(*it)->write(typesetter, &mKeywordSearch, mTopicLinks);

				if ((*it)->mCurrent_chunk < (*it)->mSplitText.size() - 1)
				{
					inChunk = true;
					mTopicsList->setVisible(false);
					MWBase::Environment::get().getInputManager()->dialogueChunkMode(true);
				}
				else {
					MWBase::Environment::get().getInputManager()->dialogueChunkMode(false);
					mTopicsList->setVisible(true);
					inChunk = false;

				}
			}
		}
		//There is still a bug here, when journal updates player sees only that notification.


        BookTypesetter::Style* body = typesetter->createStyle("", MyGUI::Colour::White);

        typesetter->sectionBreak(9);
        // choices

        const TextColours& textColours = MWBase::Environment::get().getWindowManager()->getTextColours();
        mChoices = MWBase::Environment::get().getDialogueManager()->getChoices();
		if (!inChunk) //Only show player dialogue choices when we reach the end of all chunks.
		{
			
			if (mChoices.size() == 1 && mChoices[0].first == "GO") //if there is a choice that says go, just skip to the targeting dialogue on click
			{
				MWBase::Environment::get().getInputManager()->dialogueGoMode(true);
				
			}
			else if (mChoices.size() == 2 && mChoices[0].first == "TOPIC") //if there is a topic link, just go to it, no clicking needed. Used for greetings to dive into topics.
			{
				//MWBase::Environment::get().getInputManager()->dialogueGoMode(true);
				std::string topic = mChoices[1].first;
				mChoices.clear();
				MWBase::Environment::get().getDialogueManager()->clearChoices();
				onTopicActivated(topic);
				return; 
			}
			else
			{


				for (std::vector<std::pair<std::string, int> >::const_iterator it = mChoices.begin(); it != mChoices.end(); ++it)
				{
					Choice* link = new Choice(it->second);
					link->eventChoiceActivated += MyGUI::newDelegate(this, &DialogueWindow::onChoiceActivated);
					mLinks.push_back(link);

					typesetter->lineBreak();
					BookTypesetter::Style* questionStyle = typesetter->createHotStyle(body, textColours.answer, textColours.answerOver,
						textColours.answerPressed,
						TypesetBook::InteractiveId(link));
					typesetter->write(questionStyle, to_utf8_span(it->first.c_str()));
				}
			}
		}
        mGoodbye = MWBase::Environment::get().getDialogueManager()->isGoodbye();

        if (mGoodbye)
        {
            Goodbye* link = new Goodbye();
            link->eventActivated += MyGUI::newDelegate(this, &DialogueWindow::onGoodbyeActivated);
            mLinks.push_back(link);
            std::string goodbye = MWBase::Environment::get().getWorld()->getStore().get<ESM::GameSetting>().find("sGoodbye")->getString();
            BookTypesetter::Style* questionStyle = typesetter->createHotStyle(body, textColours.answer, textColours.answerOver,
                                                                              textColours.answerPressed,
                                                                              TypesetBook::InteractiveId(link));
            typesetter->lineBreak();
            typesetter->write(questionStyle, to_utf8_span(goodbye.c_str()));
        }

        TypesetBook::Ptr book = typesetter->complete();
        mHistory->showPage(book, 0);
        size_t viewHeight = mHistory->getParent()->getHeight();
        if (!scrollbar && book->getSize().second > viewHeight)
            updateHistory(true);
        else if (scrollbar)
        {
            mHistory->setSize(MyGUI::IntSize(mHistory->getWidth(), book->getSize().second));
            size_t range = book->getSize().second - viewHeight;
            mScrollBar->setScrollRange(range);
            mScrollBar->setScrollPosition(range-1);
            mScrollBar->setTrackSize(static_cast<int>(viewHeight / static_cast<float>(book->getSize().second) * mScrollBar->getLineSize()));
            onScrollbarMoved(mScrollBar, range-1);
        }
        else
        {
            // no scrollbar
            onScrollbarMoved(mScrollBar, 0);
        }

        bool goodbyeEnabled = !MWBase::Environment::get().getDialogueManager()->isInChoice() || mGoodbye;
        bool goodbyeWasEnabled = mGoodbyeButton->getEnabled();
        mGoodbyeButton->setEnabled(goodbyeEnabled);
        if (goodbyeEnabled && !goodbyeWasEnabled)
            MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(mGoodbyeButton);

        bool topicsEnabled = !MWBase::Environment::get().getDialogueManager()->isInChoice() && !mGoodbye;
        mTopicsList->setEnabled(topicsEnabled);
	

    }

    void DialogueWindow::notifyLinkClicked (TypesetBook::InteractiveId link)
    {
        reinterpret_cast<Link*>(link)->activated();
    }

    void DialogueWindow::onTopicActivated(const std::string &topicId)
    {

		mTopicsBox->setVisible(false);
	

        MWBase::Environment::get().getDialogueManager()->keywordSelected(topicId, mCallback.get());

        updateTopics();
		//updateHistory();

    }

    void DialogueWindow::onChoiceActivated(int id)
    {
			if (id == 99)
			{
				mTopicsBox->setVisible(true);
				mTopicsList->setVisible(true);
			}
			if (id == 98)
			{
				onGoodbyeActivated();
			}
			if (id == 97)
			{
				////mPtr;
				//MWWorld::Ptr dest = MWBase::Environment::get().getAwarenessReactionsManager()->getGuardZoneOut(mPtr);
				//auto pos = dest.getCellRef().getPosition().pos;
				//onGoodbyeActivated();
				//
				//MWBase::Environment::get().getWorld()->moveObject(MWMechanics::getPlayer(), pos[0], pos[1], pos[2]);
				//auto seq = mPtr.getClass().getCreatureStats(mPtr).getAiSequence();
				//seq.clear(); //mwx fix me, not the place for all fo this stuff.
				////MWBase::Environment::get().getWorld()->advanceTime(.2, true); //mwx fix me
			}

        MWBase::Environment::get().getDialogueManager()->questionAnswered(id, mCallback.get());


        updateTopics();

    }

    void DialogueWindow::onGoodbyeActivated()
    {
        MWBase::Environment::get().getDialogueManager()->goodbyeSelected();
        MWBase::Environment::get().getWindowManager()->removeGuiMode(MWGui::GM_Dialogue);
		//MWBase::Environment::get().getWindowManager()->exitCurrentGuiMode();
        resetReference();
    }


	void DialogueWindow::nextChunk()
	{

		auto dialogue = mHistoryContents.back();

		//Queed 
		
		//We look at the most recent dialogue added to history, and iterate its current_chunk tracker. current_chunk is used to show player pieces of dialogue at a time. MWX
		
		if (dialogue->mType == message)
		{
			dialogue = mHistoryContents[mHistoryContents.size() - 2];
		}
		
		dialogue->mCurrent_chunk += 1;
		updateHistory();
	}

	void DialogueWindow::go()
	{
		
		if (mChoices.size() == 1)
		{
			onChoiceActivated(mChoices[0].second);
		}
		else
		{
			onTopicActivated(mChoices[1].first);
		}
	}

	void DialogueWindow::setPortraitImage(std::string id, std::string emotion)
	{
		
		if (MWBase::Environment::get().getWindowManager()->portraitExists(id + "/"+emotion+".dds"))
			mNpcPortrait->setImageTexture("textures\\portraits\\" + id + "\\"+emotion+".dds");
		else if (MWBase::Environment::get().getWindowManager()->portraitExists(id+"/default.dds"))
			mNpcPortrait->setImageTexture("textures\\portraits\\"+id+"\\default.dds");
		else
			mNpcPortrait->setImageTexture("textures\\portraits\\default.dds");

		mNpcPortrait->setEnabled(false);
		//mNPCPortraitBox->setEnabled(false);
	}


    void DialogueWindow::onScrollbarMoved(MyGUI::ScrollBar *sender, size_t pos)
	{
        mHistory->setPosition(0, static_cast<int>(pos) * -1);
    }

    void DialogueWindow::addResponse(const std::string &title, const std::string &text, bool needMargin)
    {
        mHistoryContents.push_back(new Response(text, title, needMargin));
        updateHistory();
        updateTopics();
    }

    void DialogueWindow::addMessageBox(const std::string& text)
    {
		//mwx got something.
        mHistoryContents.push_back(new Message(text));
        updateHistory();
    }

    void DialogueWindow::updateDisposition()
    {
        bool dispositionVisible = false;
        if (!mPtr.isEmpty() && mPtr.getClass().isNpc())
        {
            dispositionVisible = true;
            //mDispositionBar->setProgressRange(100);
            //mDispositionBar->setProgressPosition(MWBase::Environment::get().getMechanicsManager()->getDerivedDisposition(mPtr));
            //mDispositionText->setCaption(MyGUI::utility::toString(MWBase::Environment::get().getMechanicsManager()->getDerivedDisposition(mPtr))+std::string("/100"));
			//Disposition display not used in Greylock MWX
        }

       /* bool dispositionWasVisible = mDispositionBar->getVisible();

        if (dispositionVisible && !dispositionWasVisible)
        {
            mDispositionBar->setVisible(true);
            int offset = mDispositionBar->getHeight()+5;
            mTopicsList->setCoord(mTopicsList->getCoord() + MyGUI::IntCoord(0,offset,0,-offset));
            mTopicsList->adjustSize();
        }
        else if (!dispositionVisible && dispositionWasVisible)
        {
            mDispositionBar->setVisible(false);
            int offset = mDispositionBar->getHeight()+5;
            mTopicsList->setCoord(mTopicsList->getCoord() - MyGUI::IntCoord(0,offset,0,-offset));
            mTopicsList->adjustSize();
        }*/
    }

    void DialogueWindow::onReferenceUnavailable()
    {
        MWBase::Environment::get().getWindowManager()->removeGuiMode(GM_Dialogue);
    }

    void DialogueWindow::onFrame(float dt)
    {
        checkReferenceAvailable();
        if (mPtr.isEmpty())
            return;

        updateDisposition();
        deleteLater();

        if (mChoices != MWBase::Environment::get().getDialogueManager()->getChoices()
                || mGoodbye != MWBase::Environment::get().getDialogueManager()->isGoodbye())
            updateHistory();
    }

    void DialogueWindow::updateTopics()
    {
        setKeywords(MWBase::Environment::get().getDialogueManager()->getAvailableTopics());
    }

    bool DialogueWindow::isCompanion()
    {
		if (mPtr)
			return isCompanion(mPtr);
		else
			return false;
    }

    bool DialogueWindow::isCompanion(const MWWorld::Ptr& actor)
    {

        return !actor.getClass().getScript(actor).empty()
                && actor.getRefData().getLocals().getIntVar(actor.getClass().getScript(actor), "companion");

    }

}
