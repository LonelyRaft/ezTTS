
#ifndef EZTTS_EZTTS_WIN_H
#define EZTTS_EZTTS_WIN_H
#ifdef _WIN32

#include <sapi.h>
#include <string>
#include <list>
#include <map>
#include "../eztts.h"
#include "../eztts_voice.h"

class ezTTSPrivate {
    bool mSync;
    ISpVoice *mEngin;
    double mPitch;
    int mPauseCount;
    ezTTS::State mState;
    std::list<ezTTSVoice> mVoices;

    void updateVoices();

    std::map<std::wstring, std::wstring>
    getVoiceAttrs(ISpObjectToken *);

    bool isSpeaking() const;

public:
    ezTTSPrivate();

    virtual ~ezTTSPrivate();

    bool isValid() const;

    void say(const std::string &text);

    void stop();

    void pause();

    void resume();

    void setSync(bool sync);

    void setRate(double rate);

    void setPitch(double pitch);

    void setVolume(double volume);

    void setVoice(const ezTTSVoice &voice);

    void setLocale(const std::string &locale);

    bool sync() const;

    double rate() const;

    double pitch() const;

    double volume() const;

    ezTTSVoice voice() const;

    std::string locale() const;

    const std::list<ezTTSVoice> &
    availableVoices();
};

#endif // _WIN32
#endif //EZTTS_EZTTS_WIN_H
