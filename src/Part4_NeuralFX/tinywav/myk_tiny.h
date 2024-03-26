#pragma once

#include "tinywav.h"
#include <vector> 
#include <string> 


/**
 * Some helper functions sitting on top of Roth's tinywav
*/

struct myk_tiny{
  static std::vector<float> loadWav(const std::string& filename);
  static void saveWav(std::vector<float>& buffer, const int channels, const int sampleRate, const std::string& filename);
};