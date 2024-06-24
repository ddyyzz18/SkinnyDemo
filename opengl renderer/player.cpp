#include "player.h"
#define PI (3.1415926f)
#define DEGREE360 (360.0f)
Player::Player(Model* model, std::vector <Animation*>& ani)
{
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	rot = glm::vec3(0.0f, 0.0f, 0.0f);
	scl = glm::vec3(0.005f, 0.005f, 0.005f);
	m_WorldTrans = GetModel();
	m_Model = model;
	m_Moves = ani;
	m_Move = PlayerMove::IDLE;
	m_Use = true;
	m_Unable = false;
	m_MovementSpeed = 0.0f;
	m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
	m_Animator.LoadAnimation(m_Moves[0]);
}

Player::Player()
{
	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	rot = glm::vec3(0.0f, 0.0f, 0.0f);
	scl = glm::vec3(0.005f, 0.005f, 0.005f);
	m_WorldTrans = GetModel();
	m_Move = PlayerMove::IDLE;
	m_MovementSpeed = 0.0f;
	m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
	m_Use = true;

}

Player::Player(Player&& other)noexcept
{
	pos = std::move(other.pos);
	rot = std::move(other.rot);
	scl = std::move(other.scl);
	m_WorldTrans = std::move(other.m_WorldTrans);
	m_Model = std::move(other.m_Model);
	m_Moves = std::move(other.m_Moves);
	m_Blender = std::move(other.m_Blender);
	m_Move = std::move(other.m_Move);
	m_Use = std::move(other.m_Use);
	m_MovementSpeed = std::move(other.m_MovementSpeed);
	m_Front = std::move(other.m_Front);
	m_Right = std::move(other.m_Right);
	m_Animator = std::move(other.m_Animator);
	m_Unable = std::move(other.m_Unable);

	other.m_Right = glm::vec3(0.0f, 0.0f, 5.0f);
#ifdef _DEBUG
	std::cout << "moved!";
#endif
}

void Player::LoadAnimation(Animation* ani)
{
	m_Moves.push_back(std::move(ani));
}

void Player::LoadModel(Model* model)
{
	m_Model = model;
}

void Player::LoadBlender(Blender* blender)
{
	m_Blender.emplace_back(std::move(blender));
}

void Player::SetupAnimator()
{
	m_Animator.LoadAnimation(m_Moves[0]);
}

void Player::Update(float dt)
{
	if (m_Use)
	{
		UpdateAnimation(dt);

	}
}

void Player::UpdateAnimation(float dt)
{
	static int cc = 0;
	static int uturncount = 0;
	static bool blocking = false;
	if ((m_Animator.GetCurrentAni() == m_Moves[U_TURN]) && (uturncount))
	{
		ProcessTurnAround(&uturncount);
	}
	if (blocking && m_Unable)
	{
		return;
	}
	blocking = false;
	if (!(m_Animator.GetBlending()))
	{
		bool temp = false;
		temp = m_Animator.UpdateAnimation(dt);
		if (temp)
		{

			if (m_Animator.GetCurrentAni() == m_Moves[U_TURN])	//	振り返りアニメイシヨン終了時、プレイヤーを180度回転
			{

				StartBlend(ZERO_IDLE);
				uturncount++;

			}
			else if (m_Animator.GetCurrentAni() == m_Moves[ATTACK1])
			{
				StartBlend(STATIC_IDLE);
			}
			else if (m_Animator.GetCurrentAni() == m_Moves[DODGE])
			{
				StartBlend(STATIC_IDLE);
			}
			else if ((m_Animator.GetCurrentAni() == m_Moves[BLOCK]) && (m_Move == BLOCK) && (m_Unable))
			{
				blocking = true;
			}
			else
			{
				if (m_Animator.GetCurrentAni() == m_Moves[BLOCK])
					StartBlend(STATIC_IDLE);
				else
					StartBlend(IDLE);
			}
		}

	}
	else
	{
		m_Animator.UpdateAnimationBlend(dt);
	}

}

void Player::Draw(Shader& shader)
{
	if (m_Use)
		m_Model->Draw(shader);
}

void Player::BindFinalBoneMatrices(Shader& shader)
{
	//shader.use();
	std::vector<glm::mat4>& transforms = m_Animator.GetFinalBoneMatrices();
	for (int i = 0; i < transforms.size(); ++i)
	{
		shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
	}

}

void Player::BindWorldMatrix(Shader& shader)
{
	UpdateWorldTrans();
	shader.setMat4("model", m_WorldTrans);
}

void Player::ProcessKeyboard(InputState input, float deltaTime, Camera& camera)
{
	UpdatePlayerVectors(camera);
	static float newrot = rot.y;
	static float oldrot = rot.y;
	static InputState oldfacing;
	if (input != P_NONE)
		oldfacing = input;
	float velocity = m_MovementSpeed * deltaTime;
	oldrot = newrot;
	newrot = rot.y;

	if (input == InputState::P_NONE)
	{
		m_MovementSpeed -= 3.0f * ACC * deltaTime;
		if (m_MovementSpeed < 0.0f)
			m_MovementSpeed = 0.0f;
		switch (oldfacing)
		{
		case P_FORWARD:
			pos += m_Front * velocity;
			break;
		case P_BACKWARD:
			pos -= m_Front * velocity;
			break;
		case P_LEFT:
			pos -= m_Right * velocity;
			break;
		case P_RIGHT:
			pos += m_Right * velocity;
			break;

		}

	}

	//カメラの向きに合わせて　プレイヤーの向きを調整
	if (input == InputState::P_LOCK)
		*camera.GetLock() ^= true;
	if ((m_Move != U_TURN) && (m_Move != ATTACK1) && (m_Move != BLOCK))
	{
		if (input == InputState::P_FORWARD)
		{
			if (m_MovementSpeed < MAX_SPEED)
				m_MovementSpeed += ACC * deltaTime;
			else
				m_MovementSpeed = MAX_SPEED;

			pos += m_Front * velocity;
			newrot = camera.Yaw - R_ANGLE;
		}
		if (input == InputState::P_BACKWARD)
		{
			if (m_MovementSpeed < MAX_SPEED)
				m_MovementSpeed += ACC * deltaTime;
			else
				m_MovementSpeed = MAX_SPEED;

			pos -= m_Front * velocity;
			if (!*camera.GetLock())
				newrot = camera.Yaw + R_ANGLE;
			else
				newrot = camera.Yaw - R_ANGLE;
		}
		if (input == InputState::P_LEFT)
		{
			if (m_MovementSpeed < MAX_SPEED)
				m_MovementSpeed += ACC * deltaTime;
			else
				m_MovementSpeed = MAX_SPEED;

			pos -= m_Right * velocity;
			newrot = camera.Yaw;
		}

		if (input == InputState::P_RIGHT)
		{
			if (m_MovementSpeed < MAX_SPEED)
				m_MovementSpeed += ACC * deltaTime;
			else
				m_MovementSpeed = MAX_SPEED;

			pos += m_Right * velocity;
			newrot = camera.Yaw + 2.0f * R_ANGLE;
		}
	}

	oldrot = newrot - oldrot;
	if ((oldrot >= 179.0f) && (oldrot <= 181.0f) || ((oldrot >= -181.0f) && (oldrot <= -179.0f)))
	{
		StartBlend(U_TURN);
	}
	else
	{
		rot.y = newrot;
	}

	if ((m_Move == WALK_FRONT) || (m_Move == RUN_FRONT) || (m_Move == IDLE) || (m_Move == STATIC_IDLE) || (m_Move == ZERO_IDLE))
	{
		if (m_MovementSpeed >= WALK_SPEED)
			StartBlend(RUN_FRONT);
		else if (m_MovementSpeed > 0)
			StartBlend(WALK_FRONT);
		else
			StartBlend(IDLE);
	}

	if ((m_Move == U_TURN) || (m_Move == ATTACK1) || (m_Move == BLOCK) || (m_Move == DODGE))
	{
		m_MovementSpeed -= 3.0f * ACC * deltaTime;
		if (m_MovementSpeed < 0.0f)
			m_MovementSpeed = 0.0f;
		switch (oldfacing)
		{
		case P_FORWARD:
			pos += m_Front * velocity;
			break;
		case P_BACKWARD:
			pos -= m_Front * velocity;
			break;
		case P_LEFT:
			pos -= m_Right * velocity;
			break;
		case P_RIGHT:
			pos += m_Right * velocity;
			break;

		}
	}


}

void Player::UpdatePlayerVectors(Camera& camera)
{
	camera.updateCameraVectorsThird();
	auto front = camera.Front;
	auto right = camera.Right;
	front.y = 0.0f;
	right.y = 0.0f;
	m_Front = front;
	m_Right = right;

}

void Player::StartBlend(PlayerMove move)
{
	if ((m_Animator.GetCurrentAni() != m_Moves[DODGE]) || (move == STATIC_IDLE))
	{
		if (m_Animator.StartBlend(m_Blender[move]))
			m_Move = move;
	}

}

void Player::StartBlend(Blender* blender,int index)
{

	if (m_Animator.StartBlend(blender))
		m_Move = static_cast<PlayerMove>(index);

}

void Player::PlayAni(PlayerMove move)
{
	m_Animator.PlayAni(m_Blender[move]);
	m_Move = move;

}

void Player::ProcessTurnAround(int* count)
{

	if (*count)
	{

		rot.y += 180.0f;
		rot.y = fmod(rot.y, 360.0f);

		*count = 0;
	}



}

void Player::Reset()
{

	pos = glm::vec3(0.0f, 0.0f, 0.0f);
	rot = glm::vec3(0.0f, 0.0f, 0.0f);
	scl = glm::vec3(0.005f, 0.005f, 0.005f);
	m_WorldTrans = GetModel();
	m_Move = PlayerMove::IDLE;
	m_Use = true;
	m_Unable = false;
	m_MovementSpeed = 0.0f;
	m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
	m_Animator.LoadAnimation(m_Moves[0]);


}