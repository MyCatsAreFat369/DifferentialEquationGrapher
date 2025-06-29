#include <grapher/line.h>


Line::Line()
{

}


Line::Line(float x, float y, float scaleX, float scaleY) : Line::Line(x, y, scaleX, scaleY, 1.0f, 1.0f, 1.0f)
{

}

Line::Line(float x, float y, float scaleX, float scaleY, float r, float g, float b)
{
	VAO1 = new VAO();
	VAO1->Bind();

	GLfloat points[] =
	{
		0.0f, 0.0f, 0.0f,		r, g, b,
		1.0f, 1.0f, 0.0f,		r, g, b
	};

	VBO1 = new VBO(points, sizeof(points));

	VAO1->LinkAttrib(*VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1->LinkAttrib(*VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1->Unbind();
	VBO1->Unbind();

	this->x = x;
	this->y = y;
	this->scaleX = scaleX;
	this->scaleY = scaleY;
}

void Line::AttachShaders(GLuint vertexShader, GLuint fragmentShader)
{
	shader = new Shader(vertexShader, fragmentShader);

	xID = glGetUniformLocation(shader->ID, "x");
	yID = glGetUniformLocation(shader->ID, "y");
	scalexID = glGetUniformLocation(shader->ID, "scalex");
	scaleyID = glGetUniformLocation(shader->ID, "scaley");
	colorID = glGetUniformLocation(shader->ID, "flatColor");

	widthID = glGetUniformLocation(shader->ID, "width");
	heightID = glGetUniformLocation(shader->ID, "height");
}


void Line::SetColor(float r, float g, float b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}


void Line::Draw(int width, int height)
{
	if (shader == nullptr)
	{
		std::cerr << "No shader is attached to this Line object." << std::endl;
		return;
	}
	shader->Activate();

	glUniform1f(xID, x);
	glUniform1f(yID, y);
	glUniform1f(scalexID, scaleX);
	glUniform1f(scaleyID, scaleY);
	glUniform3f(colorID, r, g, b);

	glUniform1f(widthID, width);
	glUniform1f(heightID, height);

	VAO1->Bind();
	glDrawArrays(GL_LINES, 0, 2);
	VAO1->Unbind();
}

void Line::Delete()
{
	VAO1->Delete();
	VBO1->Delete();
	if(shader != nullptr) shader->Delete();

	delete VAO1;
	delete VBO1;
	if(shader != nullptr) delete shader;
}