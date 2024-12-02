#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "vector.h"

using namespace std;
using namespace chrono;

TVector<int> rabinKarp(const string& text, const string& pattern) {
    TVector<int> positions;
    int m = pattern.size();
    int n = text.size();
    int base = 256;
    int prime = 101;
    int patternHash = 0, textHash = 0, h = 1;

    for (int i = 0; i < m - 1; i++) {
        h = (h * base) % prime;
    }

    for (int i = 0; i < m; i++) {
        patternHash = (base * patternHash + pattern[i]) % prime;
        textHash = (base * textHash + text[i]) % prime;
    }

    for (int i = 0; i <= n - m; i++) {
        if (patternHash == textHash) {
            if (text.substr(i, m) == pattern) {
                positions.push_back(i);
            }
        }
        if (i < n - m) {
            textHash = (base * (textHash - text[i] * h) + text[i + m]) % prime;
            if (textHash < 0) {
                textHash += prime;
            }
        }
    }
    return positions;
}

TVector<int> knuthMorrisPratt(const string& text, const string& pattern) {
    TVector<int> positions;
    int m = pattern.size();
    int n = text.size();
    TVector<int> lps(m, 0);

    int len = 0;
    int i = 1;
    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        }
        else {
            if (len != 0) {
                len = lps[len - 1];
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }

    i = 0;
    int j = 0;
    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        if (j == m) {
            positions.push_back(i - j);
            j = lps[j - 1];
        }
        else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            }
            else {
                i++;
            }
        }
    }
    return positions;
}

void writeResults(const string& algorithmName, const string& text, const string& pattern) {
    string outputFileName = "output_" + algorithmName + ".txt";
    ofstream outputFile(outputFileName);

    if (!outputFile) {
        cout << "Error: Cannot open " << outputFileName << endl;
        return;
    }

    TVector<int> positions;
    auto start = high_resolution_clock::now();

    if (algorithmName == "Rabin-Karp") {
        outputFile << "Algorithm: Rabin-Karp" << endl;
        positions = rabinKarp(text, pattern);
    }
    else if (algorithmName == "Knuth-Morris-Pratt") {
        outputFile << "Algorithm: Knuth-Morris-Pratt" << endl;
        positions = knuthMorrisPratt(text, pattern);
    }
    else {
        cout << "Invalid algorithm name." << endl;
        return;
    }

    auto end = high_resolution_clock::now();
    double duration = duration_cast<microseconds>(end - start).count() / 1e6;

    if (positions.size() == 0) {
        outputFile << "No match found." << endl;
    }
    else {
        outputFile << "Pattern found at positions: ";
        for (int pos : positions) {
            outputFile << pos << " ";
        }
        outputFile << endl;
    }

    outputFile << "Execution time: " << duration << " seconds" << endl;
    cout << "Results for " << algorithmName << " written to " << outputFileName << endl;
}

int main() {
    ifstream inputFile("C:\\Users\\Max\\Desktop\\aicd\\input.txt");

    if (!inputFile) {
        cout << "Error: Cannot open input.txt" << endl;
        return 1;
    }

    string text, pattern;
    getline(inputFile, text);
    getline(inputFile, pattern);

    writeResults("Rabin-Karp", text, pattern);
    writeResults("Knuth-Morris-Pratt", text, pattern);

    return 0;
}
