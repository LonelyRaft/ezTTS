
#include "eztts.h"
#include <stdexcept>

#ifdef _WIN32

#include "windows/eztts_win.h"

#endif
#ifdef __linux__
#include "linux/eztts_linux.h"
#endif

void ezTTS::say(const std::string &text) {
    if (d != NULL && d->isValid()) {
        d->say(text);
    }
}

void ezTTS::stop() {
    if (d != NULL && d->isValid()) {
        d->stop();
    }
}

void ezTTS::pause() {
    if (d != NULL && d->isValid()) {
        d->pause();
    }
}

void ezTTS::resume() {
    if (d != NULL && d->isValid()) {
        d->resume();
    }
}

void ezTTS::setSync(bool sync) {
    if (d && d->isValid()) {
        d->setSync(sync);
    }
}

void ezTTS::setRate(double rate) {
    if (d != NULL && d->isValid()) {
        d->setRate(rate);
    }
}

void ezTTS::setPitch(double pitch) {
    if (d != NULL && d->isValid()) {
        d->setPitch(pitch);
    }
}

void ezTTS::setVolume(double volume) {
    if (d != NULL && d->isValid()) {
        d->setVolume(volume);
    }
}

void ezTTS::setVoice(const ezTTSVoice &voice) {
    if (d != NULL && d->isValid()) {
        d->setVoice(voice);
    }
}

void ezTTS::setLocale(const std::string &locale) {
    if (d != NULL && d->isValid()) {
        d->setLocale(locale);
    }
}

bool ezTTS::sync() const {
    if (d && d->isValid()) {
        return d->sync();
    }
    return false;
}

double ezTTS::rate() const {
    if (d != NULL && d->isValid()) {
        return d->rate();
    }
    return -1;
}

double ezTTS::pitch() const {
    if (d != NULL && d->isValid()) {
        return d->pitch();
    }
    return -1;
}

double ezTTS::volume() const {
    if (d != NULL && d->isValid()) {
        return d->volume();
    }
    return -1;
}

ezTTSVoice ezTTS::voice() const {
    ezTTSVoice voice;
    if (d && d->isValid()) {
        return d->voice();
    }
    return voice;
}

std::string ezTTS::locale() const {
    if (d && d->isValid()) {
        return d->locale();
    }
    return "";
}

const std::list<ezTTSVoice> *
ezTTS::availableVoices() {
    if (d && d->isValid()) {
        return &(d->availableVoices());
    }
    return NULL;
}

ezTTS::ezTTS() {
    d = new ezTTSPrivate;
}

ezTTS::~ezTTS() {
    delete d;
}




