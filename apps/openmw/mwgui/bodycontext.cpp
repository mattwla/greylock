#include "bodycontext.hpp"

#include <MyGUI_LanguageManager.h>
#include <MyGUI_EditBox.h>
#include <MyGUI_RenderManager.h>
#include <MyGUI_Button.h>

#include <components/misc/stringops.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/soundmanager.hpp"
#include "../mwbase/inputmanager.hpp"
#include "../mwbase/windowmanager.hpp"

#undef BodyContext

namespace MWGui
{

    BodyContextManager::BodyContextManager ()
    {
        mInterBodyContexte = NULL;
        mStaticBodyContext = NULL;
        mLastButtonPressed = -1;
        mBodyContextSpeed = .01f;
    }

    BodyContextManager::~BodyContextManager ()
    {
		if (mBodyContextes.size() > 0)
		{
			std::vector<BodyContext*>::iterator it(mBodyContextes.begin());
			for (; it != mBodyContextes.end(); ++it)
			{
				delete *it;
			}
		}
    }

    void BodyContextManager::clear()
    {
        if (mInterBodyContexte)
        {
            mInterBodyContexte->setVisible(false);

            delete mInterBodyContexte;
            mInterBodyContexte = NULL;
        }
		if (mBodyContextes.size() > 0)
		{

			std::vector<BodyContext*>::iterator it(mBodyContextes.begin());
			for (; it != mBodyContextes.end(); ++it)
			{
				if (*it == mStaticBodyContext)
					mStaticBodyContext = NULL;
				delete *it;
			}
		}
        mBodyContextes.clear();

        mLastButtonPressed = -1;
    }

    void BodyContextManager::onFrame (float frameDuration)
    {
        std::vector<BodyContext*>::iterator it;
        for(it = mBodyContextes.begin(); it != mBodyContextes.end();)
        {
            (*it)->mCurrentTime += frameDuration;
            if((*it)->mCurrentTime >= (*it)->mMaxTime && *it != mStaticBodyContext)
            {
                delete *it;
                it = mBodyContextes.erase(it);
            }
            else
                ++it;
        }

        float height = 0;
        it = mBodyContextes.begin();
        while(it != mBodyContextes.end())
        {
                (*it)->update(static_cast<int>(height));
                height += (*it)->getHeight();
                ++it;
        }

        if(mInterBodyContexte != NULL && mInterBodyContexte->mMarkedToDelete) {
            mLastButtonPressed = mInterBodyContexte->readPressedButton();
            mInterBodyContexte->setVisible(false);
            delete mInterBodyContexte;
            mInterBodyContexte = NULL;
            MWBase::Environment::get().getInputManager()->changeInputMode(
                        MWBase::Environment::get().getWindowManager()->isGuiMode());
        }
    }

    void BodyContextManager::createBodyContext (const std::string& message, bool stat)
    {
		//std::cout << "making context" << std::endl;
		BodyContext *box = new BodyContext(*this, message);
        box->mCurrentTime = 0;
        std::string realMessage = MyGUI::LanguageManager::getInstance().replaceTags(message);
        box->mMaxTime = realMessage.length()*mBodyContextSpeed;

        if(stat)
            mStaticBodyContext = box;

        mBodyContextes.push_back(box);

        if(mBodyContextes.size() > 1) {
            delete *mBodyContextes.begin();
            mBodyContextes.erase(mBodyContextes.begin());
        }

        int height = 0;
        for(std::vector<BodyContext*>::iterator it = mBodyContextes.begin(); it != mBodyContextes.end(); ++it)
        {
            (*it)->update(height);
            height += (*it)->getHeight();
        }
    }

    void BodyContextManager::removeStaticBodyContext ()
    {
        removeBodyContext(mStaticBodyContext);
        mStaticBodyContext = NULL;
    }

    bool BodyContextManager::createInteractiveBodyContext (const std::string& message, const std::vector<std::string>& buttons)
    {
        if (mInterBodyContexte != NULL)
        {
            std::cerr << "Warning: replacing an interactive message box that was not answered yet" << std::endl;
            mInterBodyContexte->setVisible(false);
            delete mInterBodyContexte;
            mInterBodyContexte = NULL;
        }

        mInterBodyContexte = new InteractiveBodyContext(*this, message, buttons);
        mLastButtonPressed = -1;

        return true;
    }

    bool BodyContextManager::isInteractiveBodyContext ()
    {
        return mInterBodyContexte != NULL;
    }


    bool BodyContextManager::removeBodyContext (BodyContext *msgbox)
    {
        std::vector<BodyContext*>::iterator it;
        for(it = mBodyContextes.begin(); it != mBodyContextes.end(); ++it)
        {
            if((*it) == msgbox)
            {
                delete (*it);
                mBodyContextes.erase(it);
                return true;
            }
        }
        return false;
    }

    int BodyContextManager::readPressedButton (bool reset)
    {
        int pressed = mLastButtonPressed;
        if (reset)
            mLastButtonPressed = -1;
        return pressed;
    }




    BodyContext::BodyContext(BodyContextManager& parBodyContextManager, const std::string& message)
      : Layout("openmw_MessageBox.layout")
      , mCurrentTime(0)
      , mMaxTime(0)
      , mBodyContextManager(parBodyContextManager)
      , mMessage(message)
    {
        // defines
        mBottomPadding = 48;
        mNextBoxPadding = 4;

        getWidget(mMessageWidget, "message");

        mMessageWidget->setCaptionWithReplacing(mMessage);
    }

    void BodyContext::update (int height)
    {
        MyGUI::IntSize gameWindowSize = MyGUI::RenderManager::getInstance().getViewSize();
        MyGUI::IntPoint pos;
        pos.left = (gameWindowSize.width - mMainWidget->getWidth())/4;
        pos.top = (gameWindowSize.height - mMainWidget->getHeight() - height - mBottomPadding);

        mMainWidget->setPosition(pos);
    }

    int BodyContext::getHeight ()
    {
        return mMainWidget->getHeight()+mNextBoxPadding;
    }



    InteractiveBodyContext::InteractiveBodyContext(BodyContextManager& parBodyContextManager, const std::string& message, const std::vector<std::string>& buttons)
        : WindowModal(MWBase::Environment::get().getWindowManager()->isGuiMode() ? "openmw_interactive_MessageBox_notransp.layout" : "openmw_interactive_BodyContext.layout")
      , mBodyContextManager(parBodyContextManager)
      , mButtonPressed(-1)
    {
        setVisible(true);

        int textPadding = 10; // padding between text-widget and main-widget
        int textButtonPadding = 10; // padding between the text-widget und the button-widget
        int buttonLeftPadding = 10; // padding between the buttons if horizontal
        int buttonTopPadding = 10; // ^-- if vertical
        int buttonLabelLeftPadding = 12; // padding between button label and button itself, from left
        int buttonLabelTopPadding = 4; // padding between button label and button itself, from top
        int buttonMainPadding = 10; // padding between buttons and bottom of the main widget

        mMarkedToDelete = false;


        getWidget(mMessageWidget, "message");
        getWidget(mButtonsWidget, "buttons");

        mMessageWidget->setSize(400, mMessageWidget->getHeight());
        mMessageWidget->setCaptionWithReplacing(message);

        MyGUI::IntSize textSize = mMessageWidget->getTextSize();

        MyGUI::IntSize gameWindowSize = MyGUI::RenderManager::getInstance().getViewSize();

        int biggestButtonWidth = 0;
        int buttonsWidth = 0;
        int buttonsHeight = 0;
        int buttonHeight = 0;
        MyGUI::IntCoord dummyCoord(0, 0, 0, 0);

        for(std::vector<std::string>::const_iterator it = buttons.begin(); it != buttons.end(); ++it)
        {
            MyGUI::Button* button = mButtonsWidget->createWidget<MyGUI::Button>(
                MyGUI::WidgetStyle::Child,
                std::string("MW_Button"),
                dummyCoord,
                MyGUI::Align::Default);
            button->setCaptionWithReplacing(*it);

            button->eventMouseButtonClick += MyGUI::newDelegate(this, &InteractiveBodyContext::mousePressed);

            mButtons.push_back(button);

            if (buttonsWidth != 0)
                buttonsWidth += buttonLeftPadding;

            int buttonWidth = button->getTextSize().width + 2*buttonLabelLeftPadding;
            buttonsWidth += buttonWidth;

            buttonHeight = button->getTextSize().height + 2*buttonLabelTopPadding;

            if (buttonsHeight != 0)
                buttonsHeight += buttonTopPadding;
            buttonsHeight += buttonHeight;

            if(buttonWidth > biggestButtonWidth)
            {
                biggestButtonWidth = buttonWidth;
            }
        }

        MyGUI::IntSize mainWidgetSize;
        if(buttonsWidth < textSize.width)
        {
            // on one line
            mainWidgetSize.width = textSize.width + 3*textPadding;
            mainWidgetSize.height = textPadding + textSize.height + textButtonPadding + buttonHeight + buttonMainPadding;

            MyGUI::IntSize realSize = mainWidgetSize +
                    // To account for borders
                    (mMainWidget->getSize() - mMainWidget->getClientWidget()->getSize());

            MyGUI::IntPoint absPos;
            absPos.left = (gameWindowSize.width - realSize.width)/2;
            absPos.top = (gameWindowSize.height - realSize.height)/2;

            mMainWidget->setPosition(absPos);
            mMainWidget->setSize(realSize);

            MyGUI::IntCoord messageWidgetCoord;
            messageWidgetCoord.left = (mainWidgetSize.width - textSize.width)/2;
            messageWidgetCoord.top = textPadding;
            mMessageWidget->setCoord(messageWidgetCoord);

            mMessageWidget->setSize(textSize);

            MyGUI::IntCoord buttonCord;
            MyGUI::IntSize buttonSize(0, buttonHeight);
            int left = (mainWidgetSize.width - buttonsWidth)/2;

            for(std::vector<MyGUI::Button*>::const_iterator button = mButtons.begin(); button != mButtons.end(); ++button)
            {
                buttonCord.left = left;
                buttonCord.top = messageWidgetCoord.top + textSize.height + textButtonPadding;

                buttonSize.width = (*button)->getTextSize().width + 2*buttonLabelLeftPadding;
                buttonSize.height = (*button)->getTextSize().height + 2*buttonLabelTopPadding;

                (*button)->setCoord(buttonCord);
                (*button)->setSize(buttonSize);

                left += buttonSize.width + buttonLeftPadding;
            }
        }
        else
        {
            // among each other
            if(biggestButtonWidth > textSize.width) {
                mainWidgetSize.width = biggestButtonWidth + buttonTopPadding*2;
            }
            else {
                mainWidgetSize.width = textSize.width + 3*textPadding;
            }

            MyGUI::IntCoord buttonCord;
            MyGUI::IntSize buttonSize(0, buttonHeight);

            int top = textPadding + textSize.height + textButtonPadding;

            for(std::vector<MyGUI::Button*>::const_iterator button = mButtons.begin(); button != mButtons.end(); ++button)
            {
                buttonSize.width = (*button)->getTextSize().width + buttonLabelLeftPadding*2;
                buttonSize.height = (*button)->getTextSize().height + buttonLabelTopPadding*2;

                buttonCord.top = top;
                buttonCord.left = (mainWidgetSize.width - buttonSize.width)/2;

                (*button)->setCoord(buttonCord);
                (*button)->setSize(buttonSize);

                top += buttonSize.height + buttonTopPadding;
            }

            mainWidgetSize.height = textPadding + textSize.height + textButtonPadding + buttonsHeight + buttonMainPadding;
            mMainWidget->setSize(mainWidgetSize +
                                 // To account for borders
                                 (mMainWidget->getSize() - mMainWidget->getClientWidget()->getSize()));

            MyGUI::IntPoint absPos;
            absPos.left = (gameWindowSize.width - mainWidgetSize.width)/2;
            absPos.top = (gameWindowSize.height - mainWidgetSize.height)/2;

            mMainWidget->setPosition(absPos);

            MyGUI::IntCoord messageWidgetCoord;
            messageWidgetCoord.left = (mainWidgetSize.width - textSize.width)/2;
            messageWidgetCoord.top = textPadding;
            messageWidgetCoord.width = textSize.width;
            messageWidgetCoord.height = textSize.height;
            mMessageWidget->setCoord(messageWidgetCoord);
        }

        // Set key focus to "Ok" button
        std::vector<std::string> keywords { "sOk", "sYes" };
        for(std::vector<MyGUI::Button*>::const_iterator button = mButtons.begin(); button != mButtons.end(); ++button)
        {
            for (const std::string& keyword : keywords)
            {
                if(Misc::StringUtils::ciEqual(MyGUI::LanguageManager::getInstance().replaceTags("#{" + keyword + "}"), (*button)->getCaption()))
                {
                    MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(*button);
                    return;
                }
            }
        }
    }

    void InteractiveBodyContext::mousePressed (MyGUI::Widget* pressed)
    {
        buttonActivated (pressed);
    }

    void InteractiveBodyContext::buttonActivated (MyGUI::Widget* pressed)
    {
        mMarkedToDelete = true;
        int index = 0;
        std::vector<MyGUI::Button*>::const_iterator button;
        for(button = mButtons.begin(); button != mButtons.end(); ++button)
        {
            if(*button == pressed)
            {
                mButtonPressed = index;
                mBodyContextManager.onButtonPressed(mButtonPressed);
                return;
            }
            index++;
        }
    }

    int InteractiveBodyContext::readPressedButton ()
    {
        return mButtonPressed;
    }

}
