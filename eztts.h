#ifndef EZTTS_EZTTS_H
#define EZTTS_EZTTS_H

#ifdef _WIN32
#ifdef EZTTS_EXPORT
#define EZTTS_API __declspec(dllexport)
#else
#define EZTTS_API __declspec(dllimport)
#endif
#else
#define EZTTS_API
#endif

#include <string>
#include <list>
#include "eztts_voice.h"

class ezTTSPrivate;

class EZTTS_API ezTTS {
    ezTTSPrivate *d;
    // disable copy
public:
    enum State {
        Ready,
        Speaking,
        Paused,
        BackendError
    };

    ezTTS();

    virtual ~ezTTS();

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

    const std::list<ezTTSVoice> *availableVoices();
};

#endif //EZTTS_EZTTS_H
