#pragma once
#ifndef VERTEX_H
#define VERTEX_H

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#define MAX_BONE_INFLUENCE 4

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

struct VertexBoneData
{
    int BoneIDs[MAX_BONE_INFLUENCE] = { -1 };
    float Weights[MAX_BONE_INFLUENCE] = { 0.0f };
    int index = 0;  //���̓Y����
 

    void AddBoneData(unsigned int BoneID, float Weight)
    {
        //�@�擾�ς݂̃{���h���X�L�b�v
        for (int i = 0; i < index; i++)
        {
            if (BoneIDs[i] == BoneID)
            {
                return;

            }
        }
        
        if (Weight == 0.0f)
        {
            return;
        }

        if (index == MAX_BONE_INFLUENCE)
        {

            assert(0);
        }

        BoneIDs[index] = BoneID;
        Weights[index] = Weight;
        index++;
    }
};

//�A�j���[�V�����Ȃ��p���_
struct BasicVertex{
    // position
    glm::vec3 Position;
    // normal-
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;

};

//skinmesh animetion�p���_
struct Vertex :public BasicVertex
{
    // tangent
    glm::vec3 Tangent;
    // bitangent
    //glm::vec3 Bitangent;

    VertexBoneData BoneData;
    Vertex();
    Vertex(glm::vec3&&, glm::vec3&&, glm::vec2&&, glm::vec3&&);
};


#endif