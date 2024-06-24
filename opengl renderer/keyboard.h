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
	static Keystate m_KeyState;		//���t���C���̃L�[�{�[�h���͏��
	static Keystate m_KeyStateLast;	//�O�t���C���̃L�[�{�[�h���͏��
public:
	KeyBoard() = default;

	//�@���͏�Ԃ��X�V
	static void UpdateKeyboard(GLFWwindow* window);


	//�@�����`�F�b�N
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

	//�@�؂�ւ��`�F�b�N
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

	//�@�����[�X�`�F�b�N
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
