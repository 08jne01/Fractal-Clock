#pragma once
class Drawable
{
public:
	Drawable();
	~Drawable();
	void createBuffers();
	virtual void updateBuffers() = 0;
	virtual void draw( unsigned int shaderProgram ) = 0;
	void destroyBuffers();

	inline unsigned int VAO() const
	{
		return m_VAO;
	}

	inline unsigned int VBO() const
	{
		return m_VBO;
	}

	inline unsigned int EBO() const
	{
		return m_EBO;
	}

private:
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_EBO;
};