#include <grapher/curve.h>


Curve::Curve(Points* points, float initialX, float initialY, float initialScaleX, float initialScaleY)
{
	// variables
	this->points = points;

	this->x = initialX;
	this->y = initialY;

	this->scaleX = initialScaleX;
	this->scaleY = initialScaleY;

	//delete indices;
}

void Curve::Flush()
{
	if(VAO1 != nullptr) delete VAO1;
	if(VBO1 != nullptr) delete VBO1;
	if(EBO1 != nullptr) delete EBO1;
}

void Curve::Generate()
{
	VAO1 = new VAO();
	VAO1->Bind();

	GLsizeiptr size = sizeof(points->points);

	VBO1 = new VBO(points->points, size);

	//GLuint* indices = new GLuint[(CURVE_POINTS_SIZE - 1) * 2];

	for (int i = 0; i < CURVE_POINTS_SIZE - 1; i++)
	{
		points->indices[2 * i] = i;
		points->indices[(2 * i) + 1] = i + 1;
	}

	GLsizeiptr sizeIndices = sizeof(points->indices);
	EBO1 = new EBO(points->indices, sizeIndices);

	VAO1->LinkAttrib(*VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1->LinkAttrib(*VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1->Unbind();
	VBO1->Unbind();
	EBO1->Unbind();
}

void Curve::AttachShaders(GLuint vertexShader, GLuint fragmentShader)
{
	shader = new Shader(vertexShader, fragmentShader);
	scalexID = glGetUniformLocation(shader->ID, "scalex");
	scaleyID = glGetUniformLocation(shader->ID, "scaley");
	xID = glGetUniformLocation(shader->ID, "x");
	yID = glGetUniformLocation(shader->ID, "y");
	colorID = glGetUniformLocation(shader->ID, "flatColor");
	
	this->vertexShader = vertexShader;
	this->fragmentShader = fragmentShader;
}

void Curve::UpdatePosition(float x, float y)
{
	this->x = x, this->y = y;
}

void Curve::UpdateScale(float scaleX, float scaleY)
{
	this->scaleX = scaleX, this->scaleY = scaleY;
}

void Curve::Draw(float* color)
{
	if (shader == nullptr)
	{
		std::cerr << "No shader is attached to this Curve object." << std::endl;
		return;
	}
	shader->Activate();
	glUniform1f(scalexID, scaleX);
	glUniform1f(scaleyID, scaleY);
	glUniform1f(xID, x);
	glUniform1f(yID, y);
	glUniform3f(colorID, color[0], color[1], color[2]);
	VAO1->Bind();
	glDrawElements(GL_LINES, (CURVE_POINTS_SIZE - 1) * 2, GL_UNSIGNED_INT, 0);
	VAO1->Unbind();
}

void Curve::Delete(bool deletePoints)
{
	VAO1->Delete();
	VBO1->Delete();
	EBO1->Delete();
	if(shader != nullptr) shader->Delete();

	delete VAO1;
	delete VBO1;
	delete EBO1;
	if(shader != nullptr) delete shader;
	if (deletePoints)
	{
		//points->Delete();
		delete points;
	}
}