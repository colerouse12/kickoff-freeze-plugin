
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/wrappers/GameEvent/ServerWrapper.h"
#include "bakkesmod/wrappers/CarWrapper.h"
#include "bakkesmod/wrappers/CanvasWrapper.h"
#include <cmath>

class KickoffFreezePlugin final : public BakkesMod::Plugin::BakkesModPlugin
{
public:
    void onLoad() override
    {
        cvar_countdown = cvarManager->registerCvar(
            "kf_countdown_secs", "3.0", "Kickoff countdown length (seconds)", true, true, 0.0f, true, 10.0f);
        cvar_autostart = cvarManager->registerCvar(
            "kf_autostart_on_reset", "1", "Auto-start countdown on training shot reset/load");

        // Hook training shot lifecycle events (covers most packs)
        gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.ResetRound",
            std::bind(&KickoffFreezePlugin::OnTrainingShotReset, this, std::placeholders::_1));
        gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.LoadRound",
            std::bind(&KickoffFreezePlugin::OnTrainingShotReset, this, std::placeholders::_1));
        gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.StartRound",
            std::bind(&KickoffFreezePlugin::OnTrainingShotReset, this, std::placeholders::_1));

        // Use renderer both for per-frame ticking and the HUD
        gameWrapper->HookRenderer(std::bind(&KickoffFreezePlugin::Render, this, std::placeholders::_1));
    }

    void onUnload() override {}

private:
    CVarWrapper cvar_countdown;
    CVarWrapper cvar_autostart;

    bool  countdownActive{false};
    float timeLeft{0.0f};

    void OnTrainingShotReset(std::string)
    {
        if (!cvar_autostart.getBoolValue()) return;
        if (!gameWrapper->IsInCustomTraining()) return;
        StartCountdown();
    }

    void StartCountdown()
    {
        timeLeft = cvar_countdown.getFloatValue();
        countdownActive = (timeLeft > 0.0f);
    }

    void TickFreeze()
    {
        if (!countdownActive) return;

        if (!gameWrapper->IsInGame() || !gameWrapper->IsInCustomTraining())
        {
            countdownActive = false;
            return;
        }

        CarWrapper car = gameWrapper->GetLocalCar();
        if (!car) return;

        // Freeze physics so there’s no creeping/rotation
        car.SetVelocity({0.f, 0.f, 0.f});
        car.SetAngularVelocity({0.f, 0.f, 0.f});

        // Block throttle, boost, jump; allow steering + powerslide
        ControllerInput ci = car.GetInput();
        ci.Throttle   = 0.0f; // block drive
        ci.Boost      = 0;    // block boost
        ci.Jump       = 0;    // block jump
        // ci.Handbrake = ci.Handbrake; // powerslide stays allowed
        // ci.Yaw/Pitch/Roll left as-is to let you aim wheels

        car.SetInput(ci);

        timeLeft -= gameWrapper->GetDeltaTime();
        if (timeLeft <= 0.f)
            countdownActive = false; // release on next frame
    }

    void Render(CanvasWrapper canvas)
    {
        if (!countdownActive) return;

        // Tick per frame via renderer
        TickFreeze();

        // Simple centered HUD: "3" "2" "1" then "GO!"
        Vector2 size = canvas.GetSize();
        std::string text = (timeLeft > 0.5f) ? std::to_string((int)std::ceil(timeLeft)) : "GO!";
        canvas.SetPosition({ size.X * 0.5f - 40.f, size.Y * 0.15f });
        canvas.DrawString(text, 4.0f, 4.0f);
    }
};

// Register plugin (the template compiles all .cpp files in src/ by default)
BAKKESMOD_PLUGIN(KickoffFreezePlugin, "Kickoff freeze for training packs", "0.1.0", PLUGINTYPE_FREEPLAY)
