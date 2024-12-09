#ifndef KMP_H
#define KMP_H

#include <vector>
#include <string>

std::vector<int> computePi(const std::string& pattern);

std::vector<int> kmp(const std::string& text, const std::string& pattern);

#endif // !KMP_H

