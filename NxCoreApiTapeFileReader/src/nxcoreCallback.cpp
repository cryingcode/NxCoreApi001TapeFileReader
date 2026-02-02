// file:  nxcoreCallback.cpp

#include <iostream>
#include <iomanip>
#include "nxcoreCallback.hpp"
#include "processNxCoreStatusMessage.hpp"

extern NxCoreClass NxCore; // defined in main.cpp

int OnNxCoreCallback(const NxCoreSystem *pNxCoreSys, const NxCoreMessage *pNxCoreMsg)
{
    switch (pNxCoreMsg->MessageType)
    {
    case NxMSG_STATUS:
        return processNxCoreStatusMessage(pNxCoreSys);

    // Future expansion points (intentionally empty for now)
    case NxMSG_EXGQUOTE:
    case NxMSG_MMQUOTE:
    case NxMSG_TRADE:
    case NxMSG_CATEGORY:
    case NxMSG_SYMBOLCHANGE:
    case NxMSG_SYMBOLSPIN:
        break;
    }

    return NxCALLBACKRETURN_CONTINUE;
}