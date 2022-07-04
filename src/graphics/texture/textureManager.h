#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// Singleton pattern (I think it's fine since this class takes over code in the main)
// I could decouple, but I really like the caching feature
// NOT THREAD SAFE
class TextureManager {
private:
  static TextureManager* m_instance;
  static unsigned int m_boundTexture;
  static std::unordered_map<std::string, unsigned int> m_textureMap; 
  TextureManager();

public:
  static TextureManager* getInstance();
  unsigned int getBoundTexture();
  std::string getMapTypeFromLocation(int location);
  void bindTextures(std::vector<std::string>& textureFilePaths, std::vector<float>& textureStrengths = std::vector<float>{ 1.0,1.0,1.0,1.0 });
};