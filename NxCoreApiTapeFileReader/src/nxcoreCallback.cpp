// nxcoreCallback.cpp
#include "nxcoreCallback.hpp"

#include "processNxCoreStatusMessage.hpp"

// extern NxCoreClass NxCore;           // still needed if you use NxCore. functions inside handlers
// Consider: passing a context object instead of relying on global in the future

[[nodiscard]] int OnNxCoreCallback(
    const NxCoreSystem *pNxCoreSys,
    const NxCoreMessage *pNxCoreMsg) noexcept
{
    // Very early exit for most frequent path (NxMSG_STATUS)
    if (pNxCoreMsg->MessageType == NxMSG_STATUS)
    {
        return processNxCoreStatusMessage(pNxCoreSys, std::cout);
    }

    // ────────────────────────────────────────────────────────────────
    // Future / optional message types — placeholders only for now
    // Add [[fallthrough]]; when intentionally falling through
    switch (pNxCoreMsg->MessageType)
    {
    case NxMSG_EXGQUOTE:
        [[fallthrough]];
    case NxMSG_MMQUOTE:
        [[fallthrough]];
    case NxMSG_TRADE:
        [[fallthrough]];
    case NxMSG_CATEGORY:
        [[fallthrough]];
    case NxMSG_SYMBOLCHANGE:
        [[fallthrough]];
    case NxMSG_SYMBOLSPIN:
        // Intentionally ignored for now — future expansion point
        break;

    default:
        // Defensive: unknown message type
        // In production you might log, count, or return NxCALLBACKRETURN_STOP
        // For now we just continue (silent ignore)
        break;
    }

    return NxCALLBACKRETURN_CONTINUE;
}