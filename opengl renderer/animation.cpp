#include "animation.h"
#include <cmath>

Animation::Animation(const std::string& animationPath, Model* model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	auto animation = scene->mAnimations[0];
	m_Duration = static_cast<float>(animation->mDuration);
	m_TicksPerSecond = static_cast<int>(animation->mTicksPerSecond);
	m_Model = model;
	aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
	globalTransformation = globalTransformation.Inverse();

	m_RootInvTransformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(globalTransformation);
	m_TotalFrames = animation->mChannels[0]->mNumPositionKeys;

	m_FPS = static_cast<unsigned int> (round(static_cast<float>(m_TotalFrames - 1) / (m_Duration / static_cast<float>(m_TicksPerSecond))));
	LoadBones(animation, *model);
	
}

Animation::~Animation()
{
#ifdef _DEBUG
	std::cout << "Destroied:";
#endif

}

//マップから骨ポインターを取得
Bone* Animation::FindBone(const std::string& name)
{
	auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
		[&](const Bone& Bone)
		{
			return Bone.GetBoneName() == name;
		}
	);
	if (iter == m_Bones.end()) return nullptr;
	else return &(*iter);
}

//骨のキーフレイム情報を取得
void Animation::LoadBones(const aiAnimation* animation, Model& model)
{

	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		auto channel = animation->mChannels[i];
		std::string name = channel->mNodeName.data;
		m_Bones.emplace_back(std::move(name), channel);
	}
	
}
