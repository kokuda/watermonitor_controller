#include <bits/unique_ptr.h>

// Initialize configuration loading
bool setupConfigFile();

// Load and return the configuration file as a string
std::unique_ptr<char[]> loadConfigFile();
