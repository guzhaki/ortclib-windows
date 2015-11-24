﻿#include "pch.h"
#include "RTCIceGatherer.h"

#include <openpeer/services/ILogger.h>
#include "webrtc/base/win32.h"

using namespace ortc_winrt_api;
using namespace Platform;

using namespace zsLib;

Windows::UI::Core::CoreDispatcher^ g_windowDispatcher;

std::string FromCx(Platform::String^ inObj) {
  return rtc::ToUtf8(inObj->Data());
}

Platform::String^ ToCx(std::string const& inObj) {
  return ref new Platform::String(rtc::ToUtf16(inObj).c_str());
}


RTCIceGatherer::RTCIceGatherer(RTCIceGatherOptions^ options) :
mNativeDelegatePointer(new RTCIceGathererDelegate())
{
  openpeer::services::ILogger::setLogLevel(zsLib::Log::Trace);
  openpeer::services::ILogger::setLogLevel("zsLib", zsLib::Log::Trace);
  openpeer::services::ILogger::setLogLevel("openpeer_services", zsLib::Log::Trace);
  openpeer::services::ILogger::setLogLevel("openpeer_services_http", zsLib::Log::Trace);
  openpeer::services::ILogger::setLogLevel("ortclib", zsLib::Log::Insane);
  openpeer::services::ILogger::setLogLevel("ortc_standup", zsLib::Log::Insane);

  //openpeer::services::ILogger::installDebuggerLogger();
  openpeer::services::ILogger::installTelnetLogger(59999, 60, true);
  zsLib::String url = zsLib::String("stun:") + "stun.vline.com";

  ortc::IICEGatherer::Server server;
  server.mURLs.push_back(url);
  ortc::IICEGatherer::Options optionsNative;
  optionsNative.mICEServers.push_back(server);

  mNativeDelegatePointer->SetOwnerObject(this);

  mNativePointer = IICEGatherer::create(mNativeDelegatePointer, optionsNative);
}

RTCIceGatherer^ RTCIceGatherer::createAssociatedGatherer()
{
  RTCIceGatherer^ ret = nullptr;
  return ret;
}

// Triggered when media is received on a new stream from remote peer.
void RTCIceGathererDelegate::onICEGathererStateChanged(
  IICEGathererPtr gatherer,
  IICEGatherer::States state
  )
{
  auto evt = ref new RTCIceGathererStateChangeEvent();
  evt->State = (RTCIceGathererState)state;
  _gatherer->OnICEGathererStateChanged(evt);
}

void RTCIceGathererDelegate::onICEGathererLocalCandidate(
  IICEGathererPtr gatherer,
  CandidatePtr candidate
  ) 
{
  auto evt = ref new RTCIceGathererCandidateEvent();
  evt->Candidate = toCX(candidate);
  _gatherer->OnICEGathererLocalCandidate(evt);
}

void RTCIceGathererDelegate::onICEGathererLocalCandidateComplete(
  IICEGathererPtr gatherer,
  CandidateCompletePtr candidate
  )
{
  auto evt = ref new RTCIceGathererCandidateCompleteEvent();
  evt->Complete = true;
  _gatherer->OnICEGathererCandidateComplete(evt);
}

void RTCIceGathererDelegate::onICEGathererLocalCandidateGone(
  IICEGathererPtr gatherer,
  CandidatePtr candidate
  )
{
  auto evt = ref new RTCIceGathererCandidateEvent();
  evt->Candidate = toCX(candidate);
  _gatherer->OnICEGathererLocalCandidateGone(evt);
}

void RTCIceGathererDelegate::onICEGathererError(
  IICEGathererPtr gatherer,
  ErrorCode errorCode,
  zsLib::String errorReason
  )
{
  auto evt = ref new RTCIceGathererErrorEvent();
  evt->Error->ErrorCode = errorCode;
  evt->Error->ErrorReason = ToCx(errorReason);
  _gatherer->OnICEGathererError(evt);
}

RTCIceCandidate^ RTCIceGathererDelegate::toCX(CandidatePtr candidate)
{
  RTCIceCandidate^ ret = ref new RTCIceCandidate();

  ret->CandidateType = (RTCIceCandidateType)candidate->mCandidateType;
  ret->Foundation = ToCx(candidate->mFoundation);
  ret->InterfaceType = ToCx(candidate->mInterfaceType);
  ret->IP = ToCx(candidate->mIP);
  ret->Port = candidate->mPort;
  ret->Priority = candidate->mPriority;
  ret->Protocol = (RTCIceProtocol)candidate->mProtocol;
  ret->RelatedAddress = ToCx(candidate->mRelatedAddress);
  ret->RelatedPort = candidate->mRelatedPort;
  ret->TCPType = (RTCIceTcpCandidateType)candidate->mTCPType;
  ret->UnfreezePriority = candidate->mUnfreezePriority;
  
  return ret;
}