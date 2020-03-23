#include "Camera.h"

Camera::Camera()
{

}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;			//Use GLfloat velocity = moveSpeed * deltaTime for same movement accross all platforms (causes lag)

	if (keys[GLFW_KEY_W])   //if W is pressed, you can also use GLFW_KEY_UP
	{
		position += front * velocity;
	}

	if (keys[GLFW_KEY_S])   //if W is pressed, you can also use GLFW_KEY_UP
	{
		position -= front * velocity;
	}

	if (keys[GLFW_KEY_A])   //if W is pressed, you can also use GLFW_KEY_UP
	{
		position -= right * velocity;
	}

	if (keys[GLFW_KEY_D])   //if W is pressed, you can also use GLFW_KEY_UP
	{
		position += right * velocity;
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}

glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(position, position + front, up);	//It will calculate a matrix which looks at something, basically all the rotations and translations from a given position
	//position - refers to the position
	//position + front = look at everything in front
}

glm::vec3 Camera::getCameraPosition()
{
	return position;
}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);			//set the magnitude of the vector to  unit in this direction, specifying how far we want to go in a particular direction

	right = glm::normalize(glm::cross(front, worldUp));			//cross product of front and worldUp
	up = glm::normalize(glm::cross(right, front));
}

Camera::~Camera()
{

}