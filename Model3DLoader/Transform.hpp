#ifndef Transform_hpp
#define Transform_hpp

struct Transform {
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);

	void setPosition(glm::vec3 position) {
		this->position = position;
	}

	void setScale(glm::vec3 scale) {
		this->scale = scale;
	}

	glm::mat4 to_mat4() {
		glm::mat4 m = glm::translate(glm::mat4(1), position);
		m *= glm::mat4_cast(glm::quat(rotation));
		m = glm::scale(m, scale);
		return m;
	}
};

#endif