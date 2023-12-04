#ifndef EZTTS_EZTTS_VOICE_H
#define EZTTS_EZTTS_VOICE_H

#include <string>

#ifdef _WIN32

class ezTTSVoice {
public:
    std::wstring mID;
    std::wstring mName;
    std::string mLocale;

    ezTTSVoice();

    ezTTSVoice(
            const std::wstring &id,
            const std::wstring &name,
            const std::string &locale
    );
};

#else

class ezTTSVoice {
public:
    std::string mID;
    std::string mName;
    std::string mLocale;

    ezTTSVoice();

    ezTTSVoice(
            const std::string &id,
            const std::string &name,
            const std::string &locale
    );
};

#endif // _WIN32

#endif //EZTTS_EZTTS_VOICE_H
