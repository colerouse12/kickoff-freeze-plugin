#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"

class KickoffFreezePlugin : public BakkesMod::Plugin::BakkesModPlugin
{
public:
    virtual void onLoad();
    virtual void onUnload();

    std::shared_ptr<CVarWrapper> cvar_countdown;
    std::shared_ptr<CVarWrapper> cvar_autostart;

private:
    bool countdownActive = false;
    float timeLeft = 0.0f;
    float lastFrameTime = 0.0f;

    void OnTrainingShotReset(std::string eventName);
    void StartCountdown();
    void TickFreeze();
    void Render(CanvasWrapper canvas);
};
