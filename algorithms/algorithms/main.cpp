#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

using namespace std;
using namespace chrono;

// Function to perform the Rabin-Karp search algorithm
vector<int> rabinKarp(const string& text, const string& pattern) {
    vector<int> positions;
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
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (text[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
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

// Function to build the prefix table (fY) for the Knuth-Morris-Pratt algorithm
void buildPrefixTable(const string& pattern, vector<int>& fY) {
    int n = pattern.size();
    fY[0] = 0;
    int j = 0;

    for (int i = 1; i < n; i++) {
        while (j > 0 && pattern[i] != pattern[j]) {
            j = fY[j - 1];
        }
        if (pattern[i] == pattern[j]) {
            j++;
        }
        fY[i] = j;
    }
}

// Knuth-Morris-Pratt search algorithm
vector<int> knuthMorrisPratt(const string& text, const string& pattern) {
    vector<int> positions;
    int n = text.size();
    int m = pattern.size();

    if (m == 0 || n == 0 || m > n) {
        return positions; // No valid search possible
    }

    vector<int> fY(m, 0);
    buildPrefixTable(pattern, fY);

    int j = 0;
    for (int i = 0; i < n; i++) {
        while (j > 0 && text[i] != pattern[j]) {
            j = fY[j - 1];
        }
        if (text[i] == pattern[j]) {
            j++;
        }
        if (j == m) {
            positions.push_back(i - m + 1); // Pattern found
            j = fY[j - 1];
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

    vector<int> positions;
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

    if (positions.empty()) {
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
