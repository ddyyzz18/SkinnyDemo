#include "timer.h"


Timer::Timer()
	:mLastTime(0.0f), mDeltaTime(0.0f), mPauseTime(0.0f), mPaused(false), mTimePaused(0.0f)
{
	mBaseTime = static_cast<float>(glfwGetTime());
}

Timer::~Timer()
{

}

void Timer::Tick()
{
	//��~������
	if (mPaused)
	{
		mDeltaTime = 0.0f;
		return;
	}

	float currentTime = static_cast<float>(glfwGetTime());	//�^�C���X�^���v�擾
	mDeltaTime = currentTime - mLastTime;					//�t���C���^�C��
	mLastTime = currentTime;

	if (mDeltaTime < 0.0f)
	{
		mDeltaTime = 0.0;
	}
}

float Timer::DeltaTime()const
{
	return mDeltaTime;
}

//�@������
void Timer::Reset()
{
	mBaseTime = static_cast<float>(glfwGetTime());
	mLastTime = mBaseTime;
	mTimePaused = 0.0f;
	mPaused = false;
}

//�@�ꎞ��~
void Timer::Pause()
{
	if (!mPaused)
	{
		mPauseTime = static_cast<float>(glfwGetTime());		//��~���_�擾
		mPaused = true;
	}
}

//�@�ĊJ
void Timer::Start()
{

	if (mPaused)
	{
		float StartFrame = static_cast<float>(glfwGetTime());
		mTimePaused += (StartFrame - mPauseTime);			//����~���Ԃ����Z
		mLastTime = StartFrame;
		mPauseTime = 0.0f;
		mPaused = false;
	}
}

//�@���v���C���Ԃ��v�Z
float Timer::TotalTime()const
{
	float currenttTime = static_cast<float>(glfwGetTime());
	if (mPaused)
	{
		return (mPauseTime - mBaseTime - mTimePaused);
	}
	else
	{
		return (currenttTime - mBaseTime - mTimePaused);
	}
}