#include "pch.h"
#include "KickoffFreezePlugin.h"
#include "bakkesmod/wrappers/includes.h"

BAKKESMOD_PLUGIN(KickoffFreezePlugin, "Kickoff freeze for training packs", "0.1.0", PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void KickoffFreezePlugin::onLoad()
{
    _globalCvarManager = cvarManager;

    cvar_countdown = std::make_shared<CVarWrapper>(cvarManager->registerCvar(
        "kf_countdown_secs", "3.0", "Kickoff countdown length (seconds)", true, true, 0.0f, true, 10.0f));

    cvar_autostart = std::make_shared<CVarWrapper>(cvarManager->registerCvar(
        "kf_autostart_on_reset", "1", "Auto-start countdown on training shot reset/load"));

    // Hook training shot lifecycle events
    gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.ResetRound",
        std::bind(&KickoffFreezePlugin::OnTrainingShotReset, this, std::placeholders::_1));
    gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.LoadRound",
        std::bind(&KickoffFreezePlugin::OnTrainingShotReset, this, std::placeholders::_1));
    gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.StartRound",
        std::bind(&KickoffFreezePlugin::OnTrainingShotReset, this, std::placeholders::_1));

    gameWrapper->RegisterDrawable(std::bind(&KickoffFreezePlugin::Render, this, std::placeholders::_1));
}

void KickoffFreezePlugin::onUnload()
{
    gameWrapper->UnregisterDrawables();
}

void KickoffFreezePlugin::OnTrainingShotReset(std::string)
{
    if (!cvar_autostart->getBoolValue()) return;
    if (!gameWrapper->IsInCustomTraining()) return;

    StartCountdown();
}

void KickoffFreezePlugin::StartCountdown()
{
    timeLeft = cvar_countdown->getFloatValue();
    countdownActive = (timeLeft > 0.0f);

    auto server = gameWrapper->GetGameEventAsServer();
    if (!server) return;

    lastFrameTime = server.GetSecondsElapsed();
}

void KickoffFreezePlugin::TickFreeze()
{
    if (!countdownActive) return;

    if (!gameWrapper->IsInGame() || !gameWrapper->IsInCustomTraining())
    {
        countdownActive = false;
        return;
    }

    CarWrapper car = gameWrapper->GetLocalCar();
    if (!car) return;

    // Freeze movement
    car.SetVelocity(Vector{ 0.f, 0.f, 0.f });




    ControllerInput ci = car.GetInput();
    ci.Throttle = 0.f;
    ci.Jump = 0;
    car.SetInput(ci);

    
    auto server = gameWrapper->GetGameEventAsServer();
    if (!server) return;

    float currentTime = server.GetSecondsElapsed();
    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    timeLeft -= deltaTime;
    if (timeLeft <= 0.f)
        countdownActive = false;
}


void KickoffFreezePlugin::Render(CanvasWrapper canvas)
{
    if (!countdownActive) return;

    TickFreeze();

    Vector2 size = canvas.GetSize();
    std::string text = (timeLeft > 0.5f) ? std::to_string((int)std::ceil(timeLeft)) : "GO!";
    canvas.SetPosition(Vector2{
    static_cast<int>(size.X * 0.5f - 40.f),
    static_cast<int>(size.Y * 0.15f)
        });

    canvas.DrawString(text, 4.0f, 4.0f);
}
