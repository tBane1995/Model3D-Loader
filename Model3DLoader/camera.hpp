#ifndef Camera_hpp
#define Camera_hpp

enum class Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

class Camera {
public:
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    // Euler Angles
    float yaw;
    float pitch;

    // camera options
    float movementSpeed;
    glm::vec2 mouseSensitivity;
    float zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f) {
        this->position = position;
        this->up = up;
        this->yaw = yaw;
        this->pitch = pitch;

        front = glm::vec3(0.0f, 0.0f, -1.0f);

        movementSpeed = 10.0f;

        int w, h;
        glfwGetWindowSize(window, &w, &h);

        mouseSensitivity.x = 180.0f / float(w);
        mouseSensitivity.y = 180.0f / float(h);

        zoom = 45.0f;

        updateCameraVectors();
    }

    ~Camera() {}

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(position, position + front, up); 
    }

    void setPosition(glm::vec3 position) {
        this->position = position;
        updateCameraVectors();
    }

    void move(glm::vec3 movement) {
        position += movement;
        updateCameraVectors();
    }

    void processKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = movementSpeed * deltaTime;

        if (direction == Camera_Movement::FORWARD)
            position += front * velocity;

        if (direction == Camera_Movement::BACKWARD)
            position -= front * velocity;

        if (direction == Camera_Movement::LEFT)
            position -= right * velocity;

        if (direction == Camera_Movement::RIGHT)
            position += right * velocity;
    }

    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= mouseSensitivity.x;
        yoffset *= mouseSensitivity.y;

        yaw += xoffset;
        pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped

        if (constrainPitch)
        {
            if (pitch > 89.0f)
                pitch = 89.0f;

            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

private:
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(f);

        // also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        //_up = glm::normalize(glm::cross(_right, _front));
    }
};

Camera* cam;

#endif