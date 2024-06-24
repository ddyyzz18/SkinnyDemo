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
	//停止中処理
	if (mPaused)
	{
		mDeltaTime = 0.0f;
		return;
	}

	float currentTime = static_cast<float>(glfwGetTime());	//タイムスタンプ取得
	mDeltaTime = currentTime - mLastTime;					//フレイムタイム
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

//　初期化
void Timer::Reset()
{
	mBaseTime = static_cast<float>(glfwGetTime());
	mLastTime = mBaseTime;
	mTimePaused = 0.0f;
	mPaused = false;
}

//　一時停止
void Timer::Pause()
{
	if (!mPaused)
	{
		mPauseTime = static_cast<float>(glfwGetTime());		//停止時点取得
		mPaused = true;
	}
}

//　再開
void Timer::Start()
{

	if (mPaused)
	{
		float StartFrame = static_cast<float>(glfwGetTime());
		mTimePaused += (StartFrame - mPauseTime);			//総停止時間を加算
		mLastTime = StartFrame;
		mPauseTime = 0.0f;
		mPaused = false;
	}
}

//　総プレイ時間を計算
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