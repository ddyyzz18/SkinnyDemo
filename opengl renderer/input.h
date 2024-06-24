#pragma once
#include <glm/glm.hpp>
/*****************************************************************************
// �}�N����`
//*****************************************************************************

// �v���O������������Ƃ��Ɏg��
#define	USE_KEYBOARD				// �錾����ƃL�[�{�[�h�ő���\�ɂȂ�
#define	USE_MOUSE					// �錾����ƃ}�E�X�ő���\�ɂȂ�
#define	USE_PAD						// �錾����ƃp�b�h�ő���\�ɂȂ�


/* game pad��� */
#define BUTTON_UP		0x00000001l	// �����L�[��(.IY<0)
#define BUTTON_DOWN		0x00000002l	// �����L�[��(.IY>0)
#define BUTTON_LEFT		0x00000004l	// �����L�[��(.IX<0)
#define BUTTON_RIGHT	0x00000008l	// �����L�[�E(.IX>0)

#define BUTTON_RUP		0x00004000l	// �����L�[��(.IRy<0)
#define BUTTON_RDOWN	0x00008000l	// �����L�[��(.IRy>0)
#define BUTTON_RLEFT	0x00016000l	// �����L�[��(.IRx<0)
#define BUTTON_RRIGHT	0x00032000l	// �����L�[�E(.IRx>0)

//#define BUTTON_A		0x00000010l	// �`�{�^��(.rgbButtons[0]&0x80)
//#define BUTTON_B		0x00000020l	// �a�{�^��(.rgbButtons[1]&0x80)
//#define BUTTON_C		0x00000040l	// �b�{�^��(.rgbButtons[2]&0x80)
//#define BUTTON_X		0x00000080l	// �w�{�^��(.rgbButtons[3]&0x80)
//#define BUTTON_Y		0x00000100l	// �x�{�^��(.rgbButtons[4]&0x80)
//#define BUTTON_Z		0x00000200l	// �y�{�^��(.rgbButtons[5]&0x80)
//#define BUTTON_L		0x00000400l	// �k�{�^��(.rgbButtons[6]&0x80)
//#define BUTTON_R		0x00000800l	// �q�{�^��(.rgbButtons[7]&0x80)
//#define BUTTON_START	0x00001000l	// �r�s�`�q�s�{�^��(.rgbButtons[8]&0x80)
//#define BUTTON_M		0x00002000l	// �l�{�^��(.rgbButtons[9]&0x80)

#define BUTTON_X		0x00000010l	// �w�{�^��(.rgbButtons[0]&0x80)
#define BUTTON_A		0x00000020l	// �`�{�^��(.rgbButtons[1]&0x80)
#define BUTTON_B		0x00000040l	// �a�{�^��(.rgbButtons[2]&0x80)
#define BUTTON_Y		0x00000080l	// �x�{�^��(.rgbButtons[3]&0x80)
#define BUTTON_L		0x00000100l	// �k�{�^��(.rgbButtons[4]&0x80)
#define BUTTON_R		0x00000200l	// �q�{�^��(.rgbButtons[5]&0x80)
#define BUTTON_L2		0x00000400l	// �k�{�^��(.rgbButtons[6]&0x80)
#define BUTTON_R2		0x00000800l	// �q�{�^��(.rgbButtons[7]&0x80)
#define BUTTON_SELECT	0x00001000l	// �y�{�^��(.rgbButtons[8]&0x80)
#define BUTTON_START	0x00002000l	// �r�s�`�q�s�{�^��(.rgbButtons[9]&0x80)

#define GAMEPADMAX		4			// �����ɐڑ�����W���C�p�b�h�̍ő吔���Z�b�g

class Input
{
enum {	// �{�^���ɑ΂���rgbButtons[?]�z��̔z��ԍ�(DirectInput��ł�XYAB�{�^���ʒu�ɍ��킹��)
	rgbButtons_X,
	rgbButtons_A,
	rgbButtons_B,
	rgbButtons_Y,
	rgbButtons_L,
	rgbButtons_R,
	rgbButtons_L2,
	rgbButtons_R2,
	rgbButtons_SELECT,
	rgbButtons_START,
};

struct stickpos
{
	long lx, ly;
	long rx, ry;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//---------------------------- keyboard
bool GetKeyboardPress(int nKey);
bool GetKeyboardTrigger(int nKey);
bool GetKeyboardRepeat(int nKey);
bool GetKeyboardRelease(int nKey);

//---------------------------- mouse
bool IsMouseLeftPressed(void);      // ���N���b�N�������
bool IsMouseLeftTriggered(void);    // ���N���b�N�����u��
bool IsMouseRightPressed(void);     // �E�N���b�N�������
bool IsMouseRightTriggered(void);   // �E�N���b�N�����u��
bool IsMouseCenterPressed(void);    // ���N���b�N�������
bool IsMouseCenterTriggered(void);  // ���N���b�N�����u��
long GetMouseX(void);               // �}�E�X��X�����ɓ��������Βl
long GetMouseY(void);               // �}�E�X��Y�����ɓ��������Βl
long GetMouseZ(void);               // �}�E�X�z�C�[�������������Βl

//---------------------------- game pad
bool IsButtonPressed(int padNo, long button);
bool IsButtonTriggered(int padNo, long button);

glm::vec2 GetMouseMove(void);
stickpos GetStickPos(int index);
};
