#include "pch.h"
#include "SpeedrunTimer.h"

using namespace constants;

// Whether or not dragging the timer to position is enabled
bool drag_enabled;

// Declare global for the timer size
extern Vector2F timer_size;
// Declare global for the screen size
extern Vector2 screen_size;

//Plugin Settings Window code here
std::string SpeedrunTimer::GetPluginName() {
	return "SpeedrunTimer";
}

void SpeedrunTimer::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

void SpeedrunTimer::RenderSettings() {
	// X position slider
	CVarWrapper x_cvar = cvarManager->getCvar(cvar_timer_x);
	if (!x_cvar) { return; }
	int timer_x = x_cvar.getIntValue();

	int max_x = screen_size.X - timer_size.X - timer_margin;
	if (ImGui::SliderInt("X Position", &timer_x, timer_margin, max_x)) {
		x_cvar.setValue(timer_x);
	}

	// Y position slider
	CVarWrapper y_cvar = cvarManager->getCvar(cvar_timer_y);
	if (!y_cvar) { return; }
	int timer_y = y_cvar.getIntValue();

	int max_y = screen_size.Y - timer_size.Y - timer_margin;
	if (ImGui::SliderInt("Y Position", &timer_y, timer_margin, max_y)) {
		y_cvar.setValue(timer_y);
	}

	// Button to reset the timer position to default
	if (ImGui::Button("Reset Position")) {
		x_cvar.setValue(timer_margin);
		y_cvar.setValue(timer_margin);
	}

	// Checkbox to enable dragging
	ImGui::Checkbox("Enable dragging (only while in settings)", &drag_enabled);

	if (drag_enabled) {
		// If a window is hovered or an item in a window is being used, do nothing
		if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive()) {
			return;
		}

		// Set cursor style
		ImGui::SetMouseCursor(2);

		// If left click is down
		if (ImGui::IsMouseDown(0)) {
			// Get the current mouse position
			ImVec2 mouse_pos = ImGui::GetMousePos();

			// Offset the position so the cursor is centered on the timer
			int new_x = mouse_pos.x - (timer_size.X / 2);
			int new_y = mouse_pos.y - (timer_size.Y / 2);

			// Force the X position to be in bounds if not already
			if (new_x < timer_margin) { new_x = timer_margin; }
			else if (new_x > max_x) { new_x = max_x; }

			// Force the Y position to be in bounds if not already
			if (new_y < timer_margin) { new_y = timer_margin; }
			else if (new_y > max_y) { new_y = max_y; }

			// Set the new position
			x_cvar.setValue(new_x);
			y_cvar.setValue(new_y);
		}
	}
}

/*
// Do ImGui rendering here
void SpeedrunTimer::Render()
{
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::End();

	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}

// Name of the menu that is used to toggle the window.
std::string SpeedrunTimer::GetMenuName()
{
	return "SpeedrunTimer";
}

// Title to give the menu
std::string SpeedrunTimer::GetMenuTitle()
{
	return menuTitle_;
}

// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
void SpeedrunTimer::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool SpeedrunTimer::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool SpeedrunTimer::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void SpeedrunTimer::OnOpen()
{
	isWindowOpen_ = true;
}

// Called when window is closed
void SpeedrunTimer::OnClose()
{
	isWindowOpen_ = false;
}
*/
