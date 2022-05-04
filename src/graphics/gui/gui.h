#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include <unordered_set>
#include <map>
#include <string>
#include <vector>



class Gui {
private:
	unsigned int guiVBO, guiVAO, guiEBO;
	unsigned int textVBO, textVAO;
	unsigned int textShaderProgram;
	bool toggleGUI = true;
	struct Character {
		unsigned int TextureID; // ID handle of the glyph texture
		glm::ivec2   Size;      // Size of glyph
		glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
		unsigned int Advance;   // Horizontal offset to advance to next glyph
	};
	std::map<char, Character> Characters;
	std::string gui_vertShaderPath = "../assets/shaders/guiShader.vs";
	std::string gui_fragShaderPath = "../assets/shaders/guiShader.fs";
	std::string text_vertShaderPath = "../assets/shaders/textShader.vs";
	std::string text_fragShaderPath = "../assets/shaders/textShader.fs";
	std::vector<std::string> planetData;

public:
	Gui();
	void render(double frameTime, bool status, std::vector<std::string> info);
	void renderText(std::string text, float x, float y, float scale, glm::vec3 color);
	unsigned int getShader();
};