#ifndef KMP_H
#define KMP_H

#include <vector>
#include <string>

std::vector<int> computePi(const std::string& pattern) {
    int m = pattern.length();
    std::vector<int> pi(m);
    pi[0] = 0;
    int k = 0;
    
    for(int i = 1; i < m; i++) {
        while(k > 0 && pattern[k] != pattern[i]) {
            k = pi[k-1];
        }
        if(pattern[k] == pattern[i]) {
            k++;
        }
        pi[i] = k;
    }
    
    return pi;
}


#endif // !KMP_H

