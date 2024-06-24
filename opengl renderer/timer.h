#pragma once
#ifndef TIMER_H
#define TIMER_H
#include <GLFW\glfw3.h>

class Timer
{
public:
	Timer();
	~Timer();
	float TotalTime()const;	// �����Ԃ��擾
	float DeltaTime()const; // �t���C���^�C�����擾
	void Reset();			// �������p
	void Start();			// �ĊJ
	void Pause();			// ��~
	void Tick();			// �t���C�����ƌĂ�
	inline bool GetPaused()	// ��~�t���O�@Getter
	{ return mPaused; }
private:
	float mLastTime;		// �O�t���C���̃^�C���X�^���v
	float mDeltaTime;		// �t���C���^�C��
	float mPauseTime;		// ��~���_�̃^�C���X�^���v
	float mBaseTime;		// �Q�[���̊J�n����
	float mTimePaused;		// ��~���_
	bool mPaused;			// �Q�[����~���H
};
#endif