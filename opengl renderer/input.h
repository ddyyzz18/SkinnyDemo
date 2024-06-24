#pragma once
#include <glm/glm.hpp>
/*****************************************************************************
// マクロ定義
//*****************************************************************************

// プログラム分けするときに使う
#define	USE_KEYBOARD				// 宣言するとキーボードで操作可能になる
#define	USE_MOUSE					// 宣言するとマウスで操作可能になる
#define	USE_PAD						// 宣言するとパッドで操作可能になる


/* game pad情報 */
#define BUTTON_UP		0x00000001l	// 方向キー上(.IY<0)
#define BUTTON_DOWN		0x00000002l	// 方向キー下(.IY>0)
#define BUTTON_LEFT		0x00000004l	// 方向キー左(.IX<0)
#define BUTTON_RIGHT	0x00000008l	// 方向キー右(.IX>0)

#define BUTTON_RUP		0x00004000l	// 方向キー上(.IRy<0)
#define BUTTON_RDOWN	0x00008000l	// 方向キー下(.IRy>0)
#define BUTTON_RLEFT	0x00016000l	// 方向キー左(.IRx<0)
#define BUTTON_RRIGHT	0x00032000l	// 方向キー右(.IRx>0)

//#define BUTTON_A		0x00000010l	// Ａボタン(.rgbButtons[0]&0x80)
//#define BUTTON_B		0x00000020l	// Ｂボタン(.rgbButtons[1]&0x80)
//#define BUTTON_C		0x00000040l	// Ｃボタン(.rgbButtons[2]&0x80)
//#define BUTTON_X		0x00000080l	// Ｘボタン(.rgbButtons[3]&0x80)
//#define BUTTON_Y		0x00000100l	// Ｙボタン(.rgbButtons[4]&0x80)
//#define BUTTON_Z		0x00000200l	// Ｚボタン(.rgbButtons[5]&0x80)
//#define BUTTON_L		0x00000400l	// Ｌボタン(.rgbButtons[6]&0x80)
//#define BUTTON_R		0x00000800l	// Ｒボタン(.rgbButtons[7]&0x80)
//#define BUTTON_START	0x00001000l	// ＳＴＡＲＴボタン(.rgbButtons[8]&0x80)
//#define BUTTON_M		0x00002000l	// Ｍボタン(.rgbButtons[9]&0x80)

#define BUTTON_X		0x00000010l	// Ｘボタン(.rgbButtons[0]&0x80)
#define BUTTON_A		0x00000020l	// Ａボタン(.rgbButtons[1]&0x80)
#define BUTTON_B		0x00000040l	// Ｂボタン(.rgbButtons[2]&0x80)
#define BUTTON_Y		0x00000080l	// Ｙボタン(.rgbButtons[3]&0x80)
#define BUTTON_L		0x00000100l	// Ｌボタン(.rgbButtons[4]&0x80)
#define BUTTON_R		0x00000200l	// Ｒボタン(.rgbButtons[5]&0x80)
#define BUTTON_L2		0x00000400l	// Ｌボタン(.rgbButtons[6]&0x80)
#define BUTTON_R2		0x00000800l	// Ｒボタン(.rgbButtons[7]&0x80)
#define BUTTON_SELECT	0x00001000l	// Ｚボタン(.rgbButtons[8]&0x80)
#define BUTTON_START	0x00002000l	// ＳＴＡＲＴボタン(.rgbButtons[9]&0x80)

#define GAMEPADMAX		4			// 同時に接続するジョイパッドの最大数をセット

class Input
{
enum {	// ボタンに対するrgbButtons[?]配列の配列番号(DirectInput上でのXYABボタン位置に合わせた)
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
// プロトタイプ宣言
//*****************************************************************************

//---------------------------- keyboard
bool GetKeyboardPress(int nKey);
bool GetKeyboardTrigger(int nKey);
bool GetKeyboardRepeat(int nKey);
bool GetKeyboardRelease(int nKey);

//---------------------------- mouse
bool IsMouseLeftPressed(void);      // 左クリックした状態
bool IsMouseLeftTriggered(void);    // 左クリックした瞬間
bool IsMouseRightPressed(void);     // 右クリックした状態
bool IsMouseRightTriggered(void);   // 右クリックした瞬間
bool IsMouseCenterPressed(void);    // 中クリックした状態
bool IsMouseCenterTriggered(void);  // 中クリックした瞬間
long GetMouseX(void);               // マウスがX方向に動いた相対値
long GetMouseY(void);               // マウスがY方向に動いた相対値
long GetMouseZ(void);               // マウスホイールが動いた相対値

//---------------------------- game pad
bool IsButtonPressed(int padNo, long button);
bool IsButtonTriggered(int padNo, long button);

glm::vec2 GetMouseMove(void);
stickpos GetStickPos(int index);
};
