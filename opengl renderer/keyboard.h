#pragma once
#ifndef KEY_H
#define KEY_H
#include <GLFW\glfw3.h>
#include <cstring> 

#define NUM_KEY_MAX (348)


struct Keystate
{
	unsigned char keystate[NUM_KEY_MAX] = { 0 };
};

class KeyBoard
{
private:
	static Keystate m_KeyState;		//今フレイムのキーボード入力状態
	static Keystate m_KeyStateLast;	//前フレイムのキーボード入力状態
public:
	KeyBoard() = default;

	//　入力状態を更新
	static void UpdateKeyboard(GLFWwindow* window);


	//　押すチェック
	static inline bool IskeyPressed(GLFWwindow* window, int keyinput)
	{
		if (m_KeyState.keystate[keyinput] == 0x80)
		{
			return true;
		}
		else
		{
			return false;
		}


	}

	//　切り替えチェック
	static inline bool IskeyTriggered(GLFWwindow* window, int keyinput)
	{

		if (((m_KeyStateLast.keystate[keyinput] ^ m_KeyState.keystate[keyinput]) & m_KeyState.keystate[keyinput]) == 0x80)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//　リリースチェック
	static inline bool IskeyReleased(GLFWwindow* window, int keyinput)
	{

		if (((m_KeyStateLast.keystate[keyinput] ^ m_KeyState.keystate[keyinput]) & ~m_KeyState.keystate[keyinput]) == 0x80)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

#endif
