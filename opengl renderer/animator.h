#pragma once
#ifndef ANIMATOR_H
#define ANIMATOR_H
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "animation.h"
#include "bone.h"

//デフォルトブレンド係数
#define BLD_FADEIN (0.4f)
#define BLD_START  (0.0f)

//アニマシオンの切り替え情報を保存
struct Blender
{
	Animation* AnimationIn;								//これから再生するアニマシオン
	float FadeInTime;									//アニマシオン切り替え済みまでの時間（秒）
	float StartTime;									//アニマシオンの何秒から再生
	bool Loop;											//ブレンドインアニマシオンをループ再生？
	bool StaticFadein;									//ブレンド中現アニマシオンを更新するか？
	bool ZeroFadein;									//ブレンドする時現アニマシオンをゼロ時点に戻る？
};

class Animator
{
private:
	std::vector<glm::mat4> m_FinalBoneMatrices;			//頂点に施す最終変化行列
	Animation* m_CurrentAnimation;						//再生してるアニマシオン

	float m_CurrentTime;								//現在時点
	float m_DeltaTime;									//フレイムタイム

	Blender* m_Blender;									//使用中のブレンド構造体
	float m_BlendInTime;								//ブレンド中次のアニマシオンの現在時点
	float m_BlenderFactor;								//ブレンド係数
	bool  m_Blending;									//アニメイシヨンのブレンドイン中？
	bool  m_Loop;										//アニマシオンをループ再生？（true：call UpdateAnimation; false: 現在アニメイシヨン一回だけ再生）
public:
	Animator(Animation* animation);
	Animator() = default;
	~Animator();

	void Showframe(int index);							//フレイム取得
	void LoadAnimation(Animation* pAnimation);			//アニマシオンを再生

	bool UpdateAnimation(float dt);						//アニマシオンの更新(ループバージョン)
	void UpdateAnimationBlend(float dt);				//アニマシオンブレンドの更新
	void UpdateAnimationBlendRot(float dt,glm::vec3 rot);
														//FinalBoneMatricesを取得
	void CalculateBoneTransform(const NodeData* node, glm::mat4& parentTransform);
	void CalculateBoneTransformBlend(const NodeData* node, glm::mat4& parentTransform);
	void CalculateBoneTransformBlendRot(const NodeData* node, glm::mat4& parentTransform, glm::vec3 rot);
														//特定フレイムのFinalBoneMatricesを取得
	void FetchFrameTransform(const NodeData* node, glm::mat4& parentTransform,int index);
	bool StartBlend(Blender* blender);
	void PlayAni(Blender* blender);
	void EndBlend();
	//void EndBlendRot(glm::vec3 rot);
	void SetCurrentTime(float dt);
	
	inline Animation* GetCurrentAni() { return m_CurrentAnimation; }
	inline float* GetCurrentTime() { return &m_CurrentTime; }
	inline bool GetBlending() {return m_Blending;}
	inline std::vector<glm::mat4>& GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}
	unsigned int GetFrameCount()
	{
		return m_CurrentAnimation->GetTotalFrames();
	}
};

#endif
