#include "eztts_voice.h"

ezTTSVoice::ezTTSVoice() {

}

#ifdef _WIN32

ezTTSVoice::ezTTSVoice(
        const std::wstring &id,
        const std::wstring &name,
        const std::string &locale) :
        mID(id), mName(name), mLocale(locale) {}

#else

ezTTSVoice::ezTTSVoice(
        const std::string &id,
        const std::string &name,
        const std::string &locale) :
        mID(id), mName(name), mLocale(locale) {}

#endif // _WIN32

