
#include "eztts.h"
#include <thread>
#include <chrono>

int main(int argc, char *argv[]) {
    ezTTS *tts = new ezTTS;
    tts->setLocale("zh");
    tts->say("恼得俺恶气生珠冠打乱");
    tts->say("不由咱一阵阵咬碎牙关");
    tts->say("手中擎起护国剑");
    tts->say("斩狂徒马前");
    tts->say("也不枉此身贬落在凡间");
    delete tts;
    return 0;
}
