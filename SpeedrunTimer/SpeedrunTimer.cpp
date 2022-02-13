#include "pch.h"
#include "SpeedrunTimer.h"

/*
A simple plugin for timing Rocket League training pack speed runs.

https://simonakers.github.io/

@author Simon Akers (Shayes)
@version 10 February 2022
*/

using namespace constants;

BAKKESMOD_PLUGIN(SpeedrunTimer, "SpeedrunTimer", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

clock_t start_time; // The time at the beginning of the training pack
clock_t last_shot_time; // The time at the end of the last shot (end of replay)

LinearColor text_color;
LinearColor bg_color;

bool timer_started = false; // Whether or not the timer is running
bool timer_shown = false; // Whether or not the timer is being rendered

double timer_time = 0; // The timer time in seconds

Vector2F timer_size;
Vector2 screen_size;

void SpeedrunTimer::onLoad() {
    _globalCvarManager = cvarManager;

    // Register CVar for the timer X position
    cvarManager->registerCvar(cvar_timer_x, std::to_string(timer_margin), "X position of the timer");
    // Register CVar for the timer Y position
    cvarManager->registerCvar(cvar_timer_y, std::to_string(timer_margin), "Y position of the timer");

    // Load values from the config file
    cvarManager->executeCommand("exec " + cfg_file_name);
    
    // Register a method for drawing
    gameWrapper -> RegisterDrawable([this](CanvasWrapper canvas) {
            Render(canvas);
        });

    // Hook to start the timer when a training pack is started
    gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.StartPlayTest",
        [this](std::string eventName) {
            StartTimer();
        });

    // Hook to stop the timer when the training pack is completed
    gameWrapper->HookEvent("Function TAGame.TrainingEditorMetrics_TA.TrainingSetComplete",
        [this](std::string eventName) {
            StopTimer();
        });

    // Hook to stop the timer when a user leaves the training pack (i.e. doesn't finish it)
    gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed",
        [this](std::string eventName) {
            StopTimer();
        });

    // Hook to log the shot time when the replay has ended
    gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.EndState",
        [this](std::string eventName) {
            LogShotTime();
        });

    // Set the screen_size variable and hook to update the screen_size variable
    // if the user changes their resolution via the menu
    screen_size = gameWrapper->GetScreenSize();
    gameWrapper->HookEvent("Function TAGame.GFxData_Settings_TA.GetResolution",
        [this](std::string eventName) {
            screen_size = gameWrapper->GetScreenSize();
        });

    SetColors();
    ShowTimer();
}

void SpeedrunTimer::onUnload() {
    WriteConfig();
}

void SpeedrunTimer::SetColors() {
    text_color.R = 255;
    text_color.G = 255;
    text_color.B = 255;
    text_color.A = 255;

    bg_color.R = 0;
    bg_color.G = 0;
    bg_color.B = 0;
    bg_color.A = 150;
}

void SpeedrunTimer::Render(CanvasWrapper canvas) {
    if (timer_shown) {
        // Get the formatted string of the timer time
        std::string fmt = FormatTime(timer_time);

        // Get the size that the background box needs to be
        timer_size = canvas.GetStringSize(fmt, 2.0, 2.0);
        timer_size.X += 10; // Add a bit of padding on the X axis

        // Draw the background box
        canvas.SetColor(bg_color);
        canvas.SetPosition(GetTimerPos());
        canvas.FillBox(timer_size);
        
        // Draw the timer time
        canvas.SetColor(text_color);
        canvas.SetPosition(GetTimerTextPos());
        canvas.DrawString(fmt, 2.0, 2.0, false);
    }

    if (timer_started) {
        // Update the timer time
        timer_time = (clock() - start_time) / (double)(CLOCKS_PER_SEC);
    }
}

void SpeedrunTimer::StartTimer() {
    timer_time = 0;

    timer_started = true;

    start_time = clock();
    last_shot_time = start_time;
}

void SpeedrunTimer::StopTimer() {
    timer_started = false;
}

void SpeedrunTimer::ShowTimer() {
    timer_shown = true;
}

void SpeedrunTimer::HideTimer() {
    timer_shown = false;
}

void SpeedrunTimer::LogShotTime() {
    std::string s = FormatTime((clock() - last_shot_time) / (double)(CLOCKS_PER_SEC));
    last_shot_time = clock();

    gameWrapper->LogToChatbox(s, "Timer Log");
}

void SpeedrunTimer::WriteConfig() {
    std::ofstream cfg;

    cfg.open(gameWrapper->GetBakkesModPath() / "cfg" / cfg_file_name);

    cfg << cvar_timer_x + " \"" + std::to_string((int)GetTimerX()) + "\"";
    cfg << "\n";
    cfg << cvar_timer_y + " \"" + std::to_string((int)GetTimerY()) + "\"";

    cfg.close();
}

float SpeedrunTimer::GetTimerX() {
    CVarWrapper cvar = cvarManager->getCvar(cvar_timer_x);

    if (!cvar) {
        return 0.0;
    }

    return cvar.getFloatValue();
}

float SpeedrunTimer::GetTimerY() {
    CVarWrapper cvar = cvarManager->getCvar(cvar_timer_y);

    if (!cvar) {
        return 0.0;
    }

    return cvar.getFloatValue();
}

Vector2F SpeedrunTimer::GetTimerPos() {
    return Vector2F{ GetTimerX(), GetTimerY() };
}

Vector2F SpeedrunTimer::GetTimerTextPos() {
    return Vector2F{ GetTimerX() + timer_text_xoff, GetTimerY() + timer_text_yoff };
}

std::string SpeedrunTimer::FormatTime(double t) {
    int mil = (int)(t * 1000) % 1000;
    int sec = (int)t % 60;
    int min = t / 60;

    return std::format("{:02}:{:02}:{:03}", min, sec, mil);
}