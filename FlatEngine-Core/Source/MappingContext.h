#pragma once
#include "Vector2.h"

#include <map>
#include <string>
#include "SDL.h"
#include <memory>
#include <vector>


namespace FlatEngine 
{
	enum PressType {
		PT_None,
		PT_Down,
		PT_Up,
		PT_Hold,
		PT_Double,
		PT_Size
	};	

	const std::vector<std::string> F_PressTypeStrings =
	{
		"None",
		"Down",
		"Up",
		"Hold",
		"Double",
	};

	enum FMouseKeys {
		Mouse_leftClick = SDL_BUTTON_LEFT,
		Mouse_middleClick = SDL_BUTTON_MIDDLE,
		Mouse_rightClick = SDL_BUTTON_RIGHT,
		Mouse_x1 = SDL_BUTTON_X1,
		Mouse_x2 = SDL_BUTTON_X2,
		Mouse_motion = 6
	};

	enum FKeyboardKeys {

		Keyboard_up = SDLK_UP,
		Keyboard_down = SDLK_DOWN,
		Keyboard_left = SDLK_LEFT,
		Keyboard_right = SDLK_RIGHT,
		Keyboard_space = SDLK_SPACE,
		Keyboard_leftCtrl = SDLK_LCTRL,
		Keyboard_leftShift = SDLK_LSHIFT,
		Keyboard_leftAlt = SDLK_LALT,
		Keyboard_rightCtrl = SDLK_RCTRL,
		Keyboard_rightShift = SDLK_RSHIFT,
		Keyboard_rightAlt = SDLK_RALT,
		Keyboard_capsLock = SDLK_CAPSLOCK,
		Keyboard_numLock = SDLK_NUMLOCKCLEAR,
		Keyboard_backspace = SDLK_BACKSPACE,
		Keyboard_escape = SDLK_ESCAPE,
		Keyboard_tab = SDLK_TAB,

		Keyboard_printScreen = SDLK_PRINTSCREEN,
		Keyboard_insert = SDLK_INSERT,
		Keyboard_home = SDLK_HOME,
		Keyboard_pageUp = SDLK_PAGEUP,
		Keyboard_pageDown = SDLK_PAGEDOWN,
		Keyboard_delete = SDLK_DELETE,

		Keyboard_backSlash = SDLK_BACKSLASH,
		Keyboard_forwardSlash = SDLK_SLASH,
		Keyboard_semicolin = SDLK_SEMICOLON,
		Keyboard_apostrophe = SDL_SCANCODE_APOSTROPHE,
		Keyboard_rightBracket = SDLK_RIGHTBRACKET,
		Keyboard_leftBracket = SDLK_LEFTBRACKET,
		Keyboard_less = SDLK_LESS,
		Keyboard_greater = SDLK_GREATER,
		Keyboard_graveAccent = SDL_SCANCODE_GRAVE,
		Keyboard_asterisk = SDLK_ASTERISK,
		Keyboard_period = SDLK_PERIOD,
		Keyboard_enter = SDLK_RETURN,
		Keyboard_0 = SDLK_0,
		Keyboard_1 = SDLK_1,
		Keyboard_2 = SDLK_2,
		Keyboard_3 = SDLK_3,
		Keyboard_4 = SDLK_4,
		Keyboard_5 = SDLK_5,
		Keyboard_6 = SDLK_6,
		Keyboard_7 = SDLK_7,
		Keyboard_8 = SDLK_8,
		Keyboard_9 = SDLK_9,
		Keyboard_minus = SDLK_MINUS,
		Keyboard_plus = SDLK_PLUS,

		Keyboard_f1 = SDLK_F1,
		Keyboard_f2 = SDLK_F2,
		Keyboard_f3 = SDLK_F3,
		Keyboard_f4 = SDLK_F4,
		Keyboard_f5 = SDLK_F5,
		Keyboard_f6 = SDLK_F6,
		Keyboard_f7 = SDLK_F7,
		Keyboard_f8 = SDLK_F8,
		Keyboard_f9 = SDLK_F9,
		Keyboard_f10 = SDLK_F10,
		Keyboard_f11 = SDLK_F11,
		Keyboard_f12 = SDLK_F12,

		Keyboard_a = SDLK_a,
		Keyboard_b = SDLK_b,
		Keyboard_c = SDLK_c,
		Keyboard_d = SDLK_d,
		Keyboard_e = SDLK_e,
		Keyboard_f = SDLK_f,
		Keyboard_g = SDLK_g,
		Keyboard_h = SDLK_h,
		Keyboard_i = SDLK_i,
		Keyboard_j = SDLK_j,
		Keyboard_k = SDLK_k,
		Keyboard_l = SDLK_l,
		Keyboard_m = SDLK_m,
		Keyboard_n = SDLK_n,
		Keyboard_o = SDLK_o,
		Keyboard_p = SDLK_p,
		Keyboard_q = SDLK_q,
		Keyboard_r = SDLK_r,
		Keyboard_s = SDLK_s,
		Keyboard_t = SDLK_t,
		Keyboard_u = SDLK_u,
		Keyboard_v = SDLK_v,
		Keyboard_w = SDLK_w,
		Keyboard_x = SDLK_x,
		Keyboard_y = SDLK_y,
		Keyboard_z = SDLK_z,
	};
	enum XInputButtons {
		A,
		B,
		X,
		Y,
		LB,
		RB,
		ScreenShot,
		Start,
		LS,
		RS,
		Home,
		Tray,
	};
	enum XInputHats {
		Hat_Up = 1,
		Hat_Down = 4,
		Hat_Left = 8,
		Hat_Right = 2,

	};
	enum XInputAxis {
		LeftXAxis,
		LeftYAxis,
		RightXAxis,
		RightYAxis,
		LT,
		RT
	};

	const std::map<long, std::string> F_MappedMouseCodes =
	{
		{ Mouse_leftClick, "Mouse: Left Click" },
		{ Mouse_rightClick, "Mouse: Right Click" },
		{ Mouse_middleClick, "Mouse: Middle Click" },
		{ Mouse_x1, "Mouse: X1" },
		{ Mouse_x2, "Mouse: X2" },
		{ Mouse_motion, "Mouse: Motion" },
	};

	const std::map<long, std::string> F_MappedKeyboardCodes =
	{
		{ Keyboard_up, "Keyboard: Arrow Up" },
		{ Keyboard_down, "Keyboard: Arrow Down" },
		{ Keyboard_left, "Keyboard: Arrow Left" },
		{ Keyboard_right, "Keyboard: Arrow Right" },
		{ Keyboard_space, "Keyboard: Space" },
		{ Keyboard_leftCtrl, "Keyboard: Left Ctrl" },
		{ Keyboard_leftShift, "Keyboard: Left Shift" },
		{ Keyboard_leftAlt, "Keyboard: Left Alt" },
		{ Keyboard_rightCtrl, "Keyboard: Right Ctrl" },
		{ Keyboard_rightShift, "Keyboard: Right Shift" },
		{ Keyboard_rightAlt, "Keyboard: Right Alt" },
		{ Keyboard_capsLock, "Keyboard: Caps Lock" },
		{ Keyboard_numLock, "Keyboard: Num Lock" },
		{ Keyboard_backspace, "Keyboard: Backspace" },
		{ Keyboard_escape, "Keyboard: Escape" },
		{ Keyboard_tab, "Keyboard: Tab" },

		{ Keyboard_printScreen, "Keyboard Print Screen" },
		{ Keyboard_insert, "Keyboard: Insert" },
		{ Keyboard_home, "Keyboard: Home" },
		{ Keyboard_pageUp, "Keyboard: Page Up" },
		{ Keyboard_pageDown, "Keyboard: Page Down" },
		{ Keyboard_delete, "Keyboard: Delete" },

		{ Keyboard_backSlash, "Keyboard: \\" },
		{ Keyboard_forwardSlash, "Keyboard: /" },
		{ Keyboard_semicolin, "Keyboard: ;" },
		{ Keyboard_apostrophe, "Keyboard: '" },
		{ Keyboard_rightBracket, "Keyboard: ]" },
		{ Keyboard_leftBracket, "Keyboard: [" },
		{ Keyboard_less, "Keyboard: <" },
		{ Keyboard_greater, "Keyboard: >" },
		{ Keyboard_graveAccent, "Keyboard: `" },
		{ Keyboard_asterisk, "Keyboard: *" },
		{ Keyboard_period, "Keyboard: ." },
		{ Keyboard_enter, "Keyboard: Enter" },
		{ Keyboard_0, "Keyboard: 0" },
		{ Keyboard_1, "Keyboard: 1" },
		{ Keyboard_2, "Keyboard: 2" },
		{ Keyboard_3, "Keyboard: 3" },
		{ Keyboard_4, "Keyboard: 4" },
		{ Keyboard_5, "Keyboard: 5" },
		{ Keyboard_6, "Keyboard: 6" },
		{ Keyboard_7, "Keyboard: 7" },
		{ Keyboard_8, "Keyboard: 8" },
		{ Keyboard_9, "Keyboard: 9" },
		{ Keyboard_minus, "Keyboard: -" },
		{ Keyboard_plus, "Keyboard: +" },

		{ Keyboard_f1, "Keyboard: F1" },
		{ Keyboard_f2, "Keyboard: F2" },
		{ Keyboard_f3, "Keyboard: F3" },
		{ Keyboard_f4, "Keyboard: F4" },
		{ Keyboard_f5, "Keyboard: F5" },
		{ Keyboard_f6, "Keyboard: F6" },
		{ Keyboard_f7, "Keyboard: F7" },
		{ Keyboard_f8, "Keyboard: F8" },
		{ Keyboard_f9, "Keyboard: F9" },
		{ Keyboard_f10, "Keyboard: F10" },
		{ Keyboard_f11, "Keyboard: F11" },
		{ Keyboard_f12, "Keyboard: F12" },

		{ Keyboard_a, "Keyboard: a" },
		{ Keyboard_b, "Keyboard: b" },
		{ Keyboard_c, "Keyboard: c" },
		{ Keyboard_d, "Keyboard: d" },
		{ Keyboard_e, "Keyboard: e" },
		{ Keyboard_f, "Keyboard: f" },
		{ Keyboard_g, "Keyboard: g" },
		{ Keyboard_h, "Keyboard: h" },
		{ Keyboard_i, "Keyboard: i" },
		{ Keyboard_j, "Keyboard: j" },
		{ Keyboard_k, "Keyboard: k" },
		{ Keyboard_l, "Keyboard: l" },
		{ Keyboard_m, "Keyboard: m" },
		{ Keyboard_n, "Keyboard: n" },
		{ Keyboard_o, "Keyboard: o" },
		{ Keyboard_p, "Keyboard: p" },
		{ Keyboard_q, "Keyboard: q" },
		{ Keyboard_r, "Keyboard: r" },
		{ Keyboard_s, "Keyboard: s" },
		{ Keyboard_t, "Keyboard: t" },
		{ Keyboard_u, "Keyboard: u" },
		{ Keyboard_v, "Keyboard: v" },
		{ Keyboard_w, "Keyboard: w" },
		{ Keyboard_x, "Keyboard: x" },
		{ Keyboard_y, "Keyboard: y" },
		{ Keyboard_z, "Keyboard: z" },
	};
	const std::map<long, std::string> F_MappedXInputButtonCodes =
	{
		{ A, "XInput: A" },
		{ B, "XInput: B" },
		{ X, "XInput: X" },
		{ Y, "XInput: Y" },
		{ LB, "XInput: Left Bumper" },
		{ RB, "XInput: Right Bumper" },
		{ ScreenShot, "XInput: Screenshot" },
		{ Start, "XInput: Start" },
		{ LS, "XInput: Left Stick Click" },
		{ RS, "XInput: Right Stick Click" },
		{ Home, "XInput: Home" },
		{ Tray, "XInput: Tray" },
	};
	const std::map<long, std::string> F_MappedXInputDPadCodes =
	{
		{ Hat_Up, "XInput: D-Pad Up" },
		{ Hat_Down, "XInput: D-Pad Down" },
		{ Hat_Left, "XInput: D-Pad Left" },
		{ Hat_Right, "XInput: D-Pad Right" }
	};
	const std::map<long, std::string> F_MappedXInputAnalogCodes =
	{
		{ LeftXAxis, "XInput: Left Joystick x-axis" },
		{ LeftYAxis, "XInput: Left Joystick y-axis" },
		{ RightXAxis, "XInput: Right Joystick x-axis" },
		{ RightYAxis, "XInput: Right Joystick y-axis" },
		{ LT, "XInput: Left Trigger" },
		{ RT, "XInput: Right Trigger" }
	};

	struct ActionMapping {
		std::string keyCode = "";
		PressType pressType = PT_None;
		SDL_Event event = SDL_Event();
		std::string actionName = "";
		bool b_fired = false;
	};

	struct PressTypeActions {
		PressTypeActions()
		{
			downAction.pressType = PT_Down;
			upAction.pressType = PT_Up;
			holdAction.pressType = PT_Hold;
			doubleAction.pressType = PT_Double;
		};
		ActionMapping downAction = ActionMapping();
		ActionMapping upAction = ActionMapping();
		ActionMapping holdAction = ActionMapping();
		ActionMapping doubleAction = ActionMapping();
	};

	struct InputMapping {
		std::string keyCode = "";
		PressTypeActions pressActions = PressTypeActions();
		std::string GetKeyCode() { return keyCode; };
		std::string GetDownActionName() { return pressActions.downAction.actionName; };
		std::string GetUpActionName() { return pressActions.upAction.actionName; };
		std::string GetHoldActionName() { return pressActions.holdAction.actionName; };
		std::string GetDoublectionName() { return pressActions.doubleAction.actionName; };
	};

	class MappingContext
	{
	public:
		MappingContext();
		~MappingContext();
		std::string GetData();

		void SetName(std::string name);
		std::string GetName();
		void SetPath(std::string path);
		std::string GetPath();
		void AddKeyBinding(std::string keyBinding, std::string actionName, PressType pressType);
		bool FireEvent(std::string actionName, SDL_Event event, PressType pressType);
		void UnFireEvent(std::string actionName);		
		SDL_Event GetInputActionEvent(std::string actionName);
		bool ActionPressed(std::string actionName);		
		std::map<std::string, std::shared_ptr<InputMapping>> GetInputActions();
		std::vector<std::shared_ptr<InputMapping>> GetInputMappingsLua();
		std::shared_ptr<InputMapping> GetInputMapping(std::string actionName);
		void SetWaitingForRemap(bool b_waiting);
		bool WaitingForRemap();
		void SetActionToRemap(std::string actionToRemap);
		std::string GetActionToRemap();
		void SetRemapStartTime(Uint32 startTime);
		bool RemapTimedOut(Uint32 currentTime);
		void SetRemapTimeoutTime(Uint32 timeoutTime);
		Uint32 GetRemapTimeoutTime();
		bool InputActionNameTaken(std::string actionName, std::string keyCode);

	private:
		std::string m_name;
		std::string m_path;
		std::map<std::string, std::shared_ptr<InputMapping>> m_inputsByBinding;
		// Eventually: map<string, vector<shared_ptr<InputMapping>> so that the same action can be bound to more than one input, ie. Controller/Keyboard can both be used for the same actions
		std::map<std::string, std::string> m_bindingsByInput;
		Uint32 m_remapTimeoutTime;
		Uint32 m_remapStartTime;
		std::string m_actionToRemap;
		bool m_b_waitingForRemap;
	};
}