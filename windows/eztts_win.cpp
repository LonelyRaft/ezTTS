
#include "eztts_win.h"

#ifdef _WIN32

#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
#include <atlcom.h>
#include <windows.h>
#include <list>

// You may derive a class from CComModule
// and use it if you want to override something,
// but do not change the name of _Module
extern CComModule _Module;

#ifndef SPERR_NO_MORE_ITEMS
# define SPERR_NO_MORE_ITEMS MAKE_SAPI_ERROR(0x039)
#endif

#ifdef __GNUC__
static const GUID CLSD_SpVoice = {
        0x96749377, 0x3391, 0x11d2,
        {0x9e, 0xe3, 0x0, 0xc0,
         0x4f, 0x79, 0x73, 0x96}
};

static inline HRESULT SpGetTokenFromId(
        const WCHAR *pszTokenId,
        ISpObjectToken **cpToken,
        BOOL fCreateIfNotExist = FALSE) {
    LPUNKNOWN pUnkOuter = nullptr;
    HRESULT hr = ::CoCreateInstance(
            CLSID_SpObjectToken, pUnkOuter,
            CLSCTX_ALL, __uuidof(ISpObjectToken),
            reinterpret_cast<void **>(cpToken));
    if (SUCCEEDED(hr)) {
        hr = (*cpToken)->SetId(
                NULL, pszTokenId,
                fCreateIfNotExist);
    }
    return hr;
}

static inline HRESULT SpCreateNewToken(
        const WCHAR *pszTokenId,
        ISpObjectToken **ppToken) {
    // Forcefully create the token
    return SpGetTokenFromId(pszTokenId, ppToken, TRUE);
}
#else

#include <sphelper.h>

#endif // __GNUC__

ezTTSPrivate::ezTTSPrivate() {
    mSync = true;
    mPitch = 0;
    mPauseCount = 0;
    mEngin = NULL;
    if (FAILED(CoInitialize(NULL))) {
        printf("%s(%d):Init of COM failed!\n",
               __FILE__, __LINE__);
        return;
    }
    HRESULT hr = CoCreateInstance(
            CLSID_SpVoice, NULL, CLSCTX_ALL,
            IID_ISpVoice, (void **) &mEngin);
    if (FAILED(hr)) {
        printf("%s(%d):Could not init voice!\n",
               __FILE__, __LINE__);
    }
    mEngin->SetInterest(SPFEI_ALL_TTS_EVENTS, SPFEI_ALL_TTS_EVENTS);
    mEngin->SetNotifyCallbackInterface(this, 0, 0);
    updateVoices();
    mState = ezTTS::Ready;
}

ezTTSPrivate::~ezTTSPrivate() {
    if (mEngin != NULL) {
        mEngin->Release();
        mEngin = NULL;
    }
    CoUninitialize();
}

void ezTTSPrivate::setSync(bool sync) {
    mSync = sync;
}

void ezTTSPrivate::setRate(double rate) {
    // -10 to 10
    mEngin->SetRate(long(rate * 10));
}

void ezTTSPrivate::setPitch(double pitch) {
    mPitch = pitch;
}

void ezTTSPrivate::setVolume(double volume) {
    // 0 to 100
    mEngin->SetVolume(volume * 100);
}

void ezTTSPrivate::setVoice(const ezTTSVoice &voice) {
    HRESULT hr = S_OK;
    ISpObjectToken *cpVoiceToken = NULL;
    if (voice.mID.empty()) {
        return;
    }
    hr = SpCreateNewToken(voice.mID.data(), &cpVoiceToken);
    if (FAILED(hr)) {
        printf("Creating the voice token from ID failed");
        if (cpVoiceToken) {
            cpVoiceToken->Release();
        }
        mState = ezTTS::BackendError;
        return;
    }
    if (mState != ezTTS::Ready) {
        mState = ezTTS::Ready;
    }
    mEngin->SetVoice(cpVoiceToken);
    cpVoiceToken->Release();
}

void ezTTSPrivate::setLocale(const std::string &locale) {
    std::list<ezTTSVoice>::const_iterator it =
            mVoices.begin();
    while (it != mVoices.end()) {
        if (it->mLocale == locale) {
            this->setVoice(*it);
        }
        it++;
    }
}

bool ezTTSPrivate::sync() const {
    return mSync;
}

double ezTTSPrivate::rate() const {
    long rateValue;
    if (mEngin->GetRate(&rateValue) == S_OK) {
        return rateValue / 10.0;
    }
    return -1;
}

double ezTTSPrivate::pitch() const {
    return mPitch;
}

double ezTTSPrivate::volume() const {
    USHORT baseVolume;
    if (mEngin->GetVolume(&baseVolume) == S_OK) {
        return baseVolume / 100.0;
    }
    return -1;
}

ezTTSVoice ezTTSPrivate::voice() const {
    ezTTSVoice ezVoice;
    ISpObjectToken *cpVoiceToken = NULL;
    mEngin->GetVoice(&cpVoiceToken);
    if (cpVoiceToken == NULL) {
        return ezVoice;
    }
    LPWSTR vId = NULL;
    HRESULT hr = cpVoiceToken->GetId(&vId);
    cpVoiceToken->Release();
    if (FAILED(hr)) {
        printf("ISpObjectToken::GetId failed");
        return ezVoice;
    }
    std::wstring voiceIdStr(vId);
    std::list<ezTTSVoice>::const_iterator it =
            mVoices.begin();
    while (it != mVoices.end()) {
        if (it->mID == voiceIdStr) {
            return *it;
        }
        it++;
    }
    return ezVoice;
}

std::string ezTTSPrivate::locale() const {
    std::string loc("");
    ISpObjectToken *cpVoiceToken = NULL;
    mEngin->GetVoice(&cpVoiceToken);
    if (cpVoiceToken == NULL) {
        return loc;
    }
    LPWSTR vId = NULL;
    HRESULT hr = cpVoiceToken->GetId(&vId);
    cpVoiceToken->Release();
    if (FAILED(hr)) {
        printf("ISpObjectToken::GetId failed");
        return loc;
    }
    std::wstring voiceIdStr(vId);
    std::list<ezTTSVoice>::const_iterator it =
            mVoices.begin();
    while (it != mVoices.end()) {
        if (it->mID == voiceIdStr) {
            return it->mLocale;
        }
        it++;
    }
    return loc;
}

void ezTTSPrivate::say(const std::string &text) {
    if (text.empty()) {
        return;
    }
    if (mState != ezTTS::Ready) {
        this->stop();
        return;
    }
    // use pitch
    char pitch[64] = {0};
    sprintf_s(pitch, 63, "<pitch middle='%d'/>", (int) (mPitch * 10));
    std::string tmp(pitch);
    tmp.append(text);
    // convert str to wstr
    std::wstring wtext;
    const int length = (int) tmp.size();
    if (length < 0) {
        return;
    }
    wchar_t *wbuff = new wchar_t[tmp.length() + 1];
    int ret = MultiByteToWideChar(
            CP_ACP, 0,
            tmp.c_str(), length,
            wbuff, length);
    if (ret < 0) {
        delete[] wbuff;
        return;
    }
    wbuff[ret] = 0;
    wtext.append(wbuff);
    delete[] wbuff;
    // play
    if (mSync) {
        mEngin->Speak(wtext.data(), SPF_DEFAULT, NULL);
    } else {
        mEngin->Speak(wtext.data(), SPF_ASYNC, NULL);
    }
}

bool ezTTSPrivate::isSpeaking() const {
    SPVOICESTATUS eventStatus;
    mEngin->GetStatus(&eventStatus, NULL);
    return eventStatus.dwRunningState == SPRS_IS_SPEAKING;
}

void ezTTSPrivate::stop() {
    if (mState == ezTTS::Paused) {
        this->resume();
    }
    mEngin->Speak(NULL, SPF_PURGEBEFORESPEAK, NULL);
}

void ezTTSPrivate::pause() {
    if (!isSpeaking()) {
        return;
    }
    if (mPauseCount == 0) {
        mPauseCount++;
        mEngin->Pause();
        mState = ezTTS::Paused;
    }
}

void ezTTSPrivate::resume() {
    if (mPauseCount <= 0) {
        return;
    }
    mPauseCount--;
    mEngin->Resume();
    if (isSpeaking()) {
        mState = ezTTS::Speaking;
    } else {
        mState = ezTTS::Ready;
    }
}

bool ezTTSPrivate::isValid() const {
    return mEngin != NULL;
}

void ezTTSPrivate::updateVoices() {
    HRESULT hr = S_OK;
    ISpObjectToken *cpVoiceToken = NULL;
    IEnumSpObjectTokens *cpEnum = NULL;
    ULONG ulCount = 0;
    hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);
    if (SUCCEEDED(hr)) {
        hr = cpEnum->GetCount(&ulCount);
    }
    mVoices.clear();
    while (SUCCEEDED(hr) && ulCount--) {
        // release last voice handle
        if (cpVoiceToken) {
            cpVoiceToken->Release();
            cpVoiceToken = NULL;
        }
        // get next voice handle
        hr = cpEnum->Next(1, &cpVoiceToken, NULL);
        // read voice attrs
        std::map<std::wstring, std::wstring> attrs =
                getVoiceAttrs(cpVoiceToken);
        // read voice id
        LPWSTR vId = NULL;
        cpVoiceToken->GetId(&vId);
        // gen a voice
        ezTTSVoice ezVoice;
        ezVoice.mID = std::wstring(vId);
        ezVoice.mName = attrs.at(L"Name");
        // find locale name
        wchar_t *flag = NULL;
        std::wstring locval = attrs.at(L"Language");
        LCID locale = wcstoul(locval.data(), &flag, 16);
        if (flag == NULL) {
            continue;
        }
        int nchars = GetLocaleInfoA(
                locale, LOCALE_SISO639LANGNAME, NULL, 0);
        if (nchars > 0) {
            char *data = new char[nchars];
            GetLocaleInfoA(locale, LOCALE_SISO639LANGNAME, data, nchars);
            ezVoice.mLocale.append(data);
            delete[] data;
        }
        // record one voice
        mVoices.push_back(ezVoice);
    }
    if (cpVoiceToken) {
        cpVoiceToken->Release();
    }
    cpEnum->Release();
}

std::map<std::wstring, std::wstring>
ezTTSPrivate::getVoiceAttrs(
        ISpObjectToken *voiceToken) {
    std::map<std::wstring, std::wstring> result;
    HRESULT hr = S_OK;
    ISpDataKey *pAttrKey = nullptr;
    hr = voiceToken->OpenKey(L"Attributes", &pAttrKey);
    if (FAILED(hr)) {
        printf("ISpObjectToken::OpenKeys failed");
        return result;
    }
    // enumerate values
    ULONG idx = 0;
    while (1) {
        LPWSTR key = NULL;
        hr = pAttrKey->EnumValues(idx, &key);
        idx++;
        if (SPERR_NO_MORE_ITEMS == hr) {
            break; // done
        } else if (FAILED(hr)) {
            printf("ISpDataKey::EnumValues failed");
            continue;
        }
        // how do we know whether it's a string or a DWORD?
        LPWSTR value = NULL;
        hr = pAttrKey->GetStringValue(key, &value);
        if (FAILED(hr)) {
            printf("ISpDataKey::GetStringValue failed");
            continue;
        }
        if (0 != wcscmp(key, L"")) {
            result[std::wstring(key)] = std::wstring(value);
        }
        // FIXME: Do we need to free the memory here?
        CoTaskMemFree(key);
        CoTaskMemFree(value);
    }
    return result;
}

const std::list<ezTTSVoice> &
ezTTSPrivate::availableVoices() {
    return mVoices;
}

HRESULT ezTTSPrivate::NotifyCallback(WPARAM, LPARAM) {
    if (mPauseCount) {
        mState = ezTTS::Paused;
    } else if (isSpeaking()) {
        mState = ezTTS::Speaking;
    } else {
        mState = ezTTS::Ready;
    }
    return S_OK;
}

#endif // _WIN32
