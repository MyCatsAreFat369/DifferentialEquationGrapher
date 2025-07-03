#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>

class EBO
{
	public:
		GLuint ID;
		EBO(GLsizeiptr size);

		void Buffer(GLuint* indices);

		void Bind();
		void Unbind();
		void Delete();
	private:
		GLsizeiptr size;
};

#endif