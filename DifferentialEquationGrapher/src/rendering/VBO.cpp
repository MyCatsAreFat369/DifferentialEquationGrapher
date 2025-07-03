#include "rendering/VBO.h"

VBO::VBO(GLsizeiptr size)
{
	this->size = size;

	glGenBuffers(1, &ID);

	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
	Unbind();
}

void VBO::Buffer(GLfloat* vertices)
{
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
}

void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}