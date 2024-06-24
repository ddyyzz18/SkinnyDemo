#include "keyboard.h"

//Ã“Iƒƒ“ƒo[•Ï”‚Ì‰Šú‰»
Keystate KeyBoard::m_KeyState;
Keystate KeyBoard::m_KeyStateLast;

void KeyBoard::UpdateKeyboard(GLFWwindow* window)
{
	memcpy(m_KeyStateLast.keystate, m_KeyState.keystate, NUM_KEY_MAX);
	for (int keyinput = 0; keyinput < NUM_KEY_MAX; keyinput++)
	{
		if (glfwGetKey(window, keyinput) == GLFW_PRESS)
		{
			m_KeyState.keystate[keyinput] = 0x80;
		}
		else
		{
			m_KeyState.keystate[keyinput] = 0x00;
		}
	}

}