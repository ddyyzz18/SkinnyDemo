#include "camera.h"



Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float roll) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSI), Zoom(ZOOM)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	Roll = roll;
	m_Lock = false;
	m_Mode = THIRD_PERSON;
	m_Focus = NULL;
	m_Len = 0.0f;
	updateCameraVectorsFree();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float roll) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSI), Zoom(ZOOM)
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	Roll = roll;
	m_Lock = false;
	m_Mode = THIRD_PERSON;
	m_Focus = NULL;
	m_Len = 0.0f;
	updateCameraVectorsFree();
}

//�@�L�[�{�[�h���͏���
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == Camera_Movement::C_FORWARD)
		Position += Front * velocity;
	if (direction == Camera_Movement::C_BACKWARD)
		Position -= Front * velocity;
	if (direction == Camera_Movement::C_LEFT)
		Position -= Right * velocity;
	if (direction == Camera_Movement::C_RIGHT)
		Position += Right * velocity;

}

//�@�}�E�X���͏���
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	if (m_Mode == FREE)
	{
		Yaw += xoffset;
		Pitch += yoffset;

		// ��ʔ��]��h��
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
		// �J���������̍X�V
		updateCameraVectorsFree();
	}
	else if (m_Mode == THIRD_PERSON)
	{
		Yaw -= xoffset;
		Pitch -= yoffset;

		// ��ʔ��]��h��
		if (constrainPitch)
		{
			if (Pitch > 45.0f)
				Pitch = 45.0f;
			if (Pitch < -30.0f)
				Pitch = -30.0f;
		}
		updateCameraVectorsThird();
	}



}

//�@�}�E�X�X�N���[������
void Camera::ProcessMouseScroll(float yoffset)
{

	Zoom -= (float)yoffset;
	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;


}

glm::mat4 Camera::My_LookAt_Matrix(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
{

	// 1. Position ���͍ς�
	// 
	// 2. ��냔�F�N�g���@�i�����O�O�C�O�C�|�P�j
	glm::vec3 zaxis = glm::normalize(position - target);
	// 3. �E���F�N�g��
	glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(worldUp), zaxis));
	// 4. �ピ�F�N�g��
	glm::vec3 yaxis = glm::cross(zaxis, xaxis);

	// ���s�ړ��ƃ��[�e�C�V����
	// glm�ł́A mat[col][row] 
	glm::mat4 translation = glm::mat4(1.0f);
	translation[3][0] = -position.x; // Third column, first row
	translation[3][1] = -position.y;
	translation[3][2] = -position.z;

	glm::mat4 rotation = glm::mat4(1.0f);
	rotation[0][0] = xaxis.x; // First column, first row
	rotation[1][0] = xaxis.y;
	rotation[2][0] = xaxis.z;
	rotation[0][1] = yaxis.x; // First column, second row
	rotation[1][1] = yaxis.y;
	rotation[2][1] = yaxis.z;
	rotation[0][2] = zaxis.x; // First column, third row
	rotation[1][2] = zaxis.y;
	rotation[2][2] = zaxis.z;
	// ��]�s��͒����s��@�[���@transpose of rot = rot^(-1)
	// 
	// View�s����擾
	return rotation * translation; //
}

void Camera::BindCamera(Shader& shader)
{
	shader.use();
	glm::mat4 projection = glm::perspective(glm::radians(Zoom), (float)ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);
	glm::mat4 view = GetViewMatrix();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setVec3("viewPos", Position);
}