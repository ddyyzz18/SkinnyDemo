#pragma once
#ifndef TIMER_H
#define TIMER_H
#include <GLFW\glfw3.h>

class Timer
{
public:
	Timer();
	~Timer();
	float TotalTime()const;	// 総時間を取得
	float DeltaTime()const; // フレイムタイムを取得
	void Reset();			// 初期化用
	void Start();			// 再開
	void Pause();			// 停止
	void Tick();			// フレイムごと呼ぶ
	inline bool GetPaused()	// 停止フラグ　Getter
	{ return mPaused; }
private:
	float mLastTime;		// 前フレイムのタイムスタンプ
	float mDeltaTime;		// フレイムタイム
	float mPauseTime;		// 停止時点のタイムスタンプ
	float mBaseTime;		// ゲームの開始時間
	float mTimePaused;		// 停止時点
	bool mPaused;			// ゲーム停止中？
};
#endif