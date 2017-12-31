#ifndef MWGUI_BODY_CONTEXT_H
#define MWGUI_BODY_CONTEXT_H

#include "windowbase.hpp"

#undef BodyContext

namespace MyGUI
{
    class Widget;
    class Button;
    class EditBox;
}

namespace MWGui
{
    class InteractiveBodyContext;
    class BodyContextManager;
    class BodyContext;
    class BodyContextManager
    {
        public:
            BodyContextManager ();
            ~BodyContextManager ();
            void onFrame (float frameDuration);
            void createBodyContext (const std::string& message, bool stat = false);
            void removeStaticBodyContext ();
            bool createInteractiveBodyContext (const std::string& message, const std::vector<std::string>& buttons);
            bool isInteractiveBodyContext ();

            /// Remove all message boxes
            void clear();

            bool removeBodyContext (BodyContext *msgbox);

            /// @param reset Reset the pressed button to -1 after reading it.
            int readPressedButton (bool reset=true);

            typedef MyGUI::delegates::CMultiDelegate1<int> EventHandle_Int;

            // Note: this delegate unassigns itself after it was fired, i.e. works once.
            EventHandle_Int eventButtonPressed;

            void onButtonPressed(int button) { eventButtonPressed(button); eventButtonPressed.clear(); }

        private:
            std::vector<BodyContext*> mBodyContextes;
            InteractiveBodyContext* mInterBodyContexte;
            BodyContext* mStaticBodyContext;
            float mBodyContextSpeed;
            int mLastButtonPressed;
    };

    class BodyContext : public Layout
    {
        public:
            BodyContext (BodyContextManager& parBodyContextManager, const std::string& message);
            void setMessage (const std::string& message);
            int getHeight ();
            void update (int height);

            float mCurrentTime;
            float mMaxTime;

        protected:
            BodyContextManager& mBodyContextManager;
            const std::string& mMessage;
            MyGUI::EditBox* mMessageWidget;
            int mBottomPadding;
            int mNextBoxPadding;
    };

    class InteractiveBodyContext : public WindowModal
    {
        public:
            InteractiveBodyContext (BodyContextManager& parBodyContextManager, const std::string& message, const std::vector<std::string>& buttons);
            void mousePressed (MyGUI::Widget* _widget);
            int readPressedButton ();

            virtual bool exit() { return false; }

            bool mMarkedToDelete;

        private:
            void buttonActivated (MyGUI::Widget* _widget);

            BodyContextManager& mBodyContextManager;
            MyGUI::EditBox* mMessageWidget;
            MyGUI::Widget* mButtonsWidget;
            std::vector<MyGUI::Button*> mButtons;

            int mButtonPressed;
    };

}

#endif
