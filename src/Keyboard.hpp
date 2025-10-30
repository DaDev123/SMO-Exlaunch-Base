#pragma once

#include <cstddef>
#include "Library/Thread/AsyncFunctorThread.h"
#include "Library/Thread/FunctorV0M.h"

#include "NintendoSDK/nn/swkbd/swkbd.h"

typedef void (*KeyboardSetup)(nn::swkbd::KeyboardConfig&);

class Keyboard {
    public:
        Keyboard(ulong strSize);
        void keyboardThread();

        void openKeyboard(const char* initialText, KeyboardSetup setup);

        const char* getResult() {
            if (mThread->isDone()) {
                return mResultString.cstr();
            }
            return nullptr;
        };

        bool isKeyboardCancelled() const { return mIsCancelled; }

        bool isThreadDone() { return mThread->isDone(); }

        void setHeaderText(const char16_t* text) { mHeaderText = text; }
        void setSubText(const char16_t* text) { mSubText = text; }

    private:

        al::AsyncFunctorThread* mThread;
        nn::swkbd::String mResultString;

        hostname mInitialText;
        KeyboardSetup mSetupFunc;

        const char16_t *mHeaderText = u"Enter Server IP Here!";
        const char16_t* mSubText = u"Must be a Valid Address.";

        bool mIsCancelled = false;
        
        char* mWorkBuf;
        int mWorkBufSize;
        char* mTextCheckBuf;
        int mTextCheckSize;
        char* mCustomizeDicBuf;
        int mCustomizeDicSize;
};