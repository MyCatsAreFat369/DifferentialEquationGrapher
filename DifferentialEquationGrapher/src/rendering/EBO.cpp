#include "rendering/EBO.h"

EBO::EBO(GLsizeiptr size)
{
	this->size = size;

	glGenBuffers(1, &ID);

	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
	Unbind();
}

void EBO::Buffer(GLuint* indices)
{
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, indices);
}

void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}