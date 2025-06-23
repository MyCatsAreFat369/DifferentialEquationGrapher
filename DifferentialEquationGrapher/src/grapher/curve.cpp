#include <grapher/curve.h>


Curve::Curve(Points* points, float initialScaleX, float initialScaleY)
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

	EBO1 = new EBO(points->indices, size);

	VAO1->LinkAttrib(*VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1->LinkAttrib(*VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1->Unbind();
	VBO1->Unbind();
	EBO1->Unbind();


	// variables
	this->points = points;

	this->scaleX = initialScaleX;
	this->scaleY = initialScaleY;

	//delete indices;
}

void Curve::AttachShaders(GLuint vertexShader, GLuint fragmentShader)
{
	shader = new Shader(vertexShader, fragmentShader);
	scalexID = glGetUniformLocation(shader->ID, "scalex");
	scaleyID = glGetUniformLocation(shader->ID, "scaley");
	xID = glGetUniformLocation(shader->ID, "x");
	yID = glGetUniformLocation(shader->ID, "y");
	
	this->vertexShader = vertexShader;
	this->fragmentShader = fragmentShader;
}

void Curve::Draw()
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
	VAO1->Bind();
	glDrawElements(GL_LINES, (CURVE_POINTS_SIZE - 1) * 2, GL_UNSIGNED_INT, 0);
	VAO1->Unbind();
}

void Curve::Delete()
{
	VAO1->Delete();
	VBO1->Delete();
	EBO1->Delete();
	if(shader != nullptr) shader->Delete();

	delete VAO1;
	delete VBO1;
	delete EBO1;
	if(shader != nullptr) delete shader;
	delete points;
}