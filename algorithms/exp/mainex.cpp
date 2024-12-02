#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <random>

int* SFT_TRIVIAL(const std::string& text, const std::string& pattern, int& occurrencesCount) {
    size_t n = text.size(), m = pattern.size();
    int* occurrences = new int[n];
    occurrencesCount = 0;

    for (size_t i = 0; i <= n - m; ++i) {
        if (text.substr(i, m) == pattern) {
            occurrences[occurrencesCount++] = i;
        }
    }

    return occurrences;
}

int* buildLPS(const std::string& pattern, size_t& length) {
    size_t m = pattern.size();
    int* lps = new int[m];
    lps[0] = 0;
    length = m;

    size_t len = 0;
    for (size_t i = 1; i < m;) {
        if (pattern[i] == pattern[len]) {
            lps[i++] = ++len;
        }
        else if (len) {
            len = lps[len - 1];
        }
        else {
            lps[i++] = 0;
        }
    }
    return lps; 
}

int* SFT_KMP(const std::string& text, const std::string& pattern, int& occurrencesCount) {
    size_t i = 0, j = 0, n = text.size(), m = pattern.size();
    occurrencesCount = 0;
    int* occurrences = new int[n]; 
    int* lps = buildLPS(pattern, m);

    while (i < n) {
        if (text[i] == pattern[j]) {
            ++i;
            ++j;
        }
        if (j == m) {
            occurrences[occurrencesCount++] = i - j;
            j = lps[j - 1];
        }
        else if (i < n && text[i] != pattern[j]) {
            if (j) {
                j = lps[j - 1];
            }
            else {
                ++i;
            }
        }
    }

    delete[] lps;
    return occurrences; 
}

std::string generateRandomString(int length, int alphabetSize) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, alphabetSize - 1);
    std::string text;
    for (int i = 0; i < length; ++i) {
        text += 'a' + dist(rng);
    }
    return text;
}

void performExperiment(int experimentChoice) {
    using namespace std::chrono;

    if (experimentChoice == 1) {
        std::cout << "Running Experiment 1: Y=(ab)^k, X=(ab)^1000*k, k=1,...,1001 with step 10\n";
        for (int k = 1; k <= 1001; k += 10) {
            std::string text = generateRandomString(1000 * k, 2);  // X=(ab)^1000*k
            std::string pattern = "ab";  // Y=(ab)^k

            std::cout << "k=" << k << ": Working...\n";
            int occurrencesCount;
            auto start = high_resolution_clock::now();
            int* occurrences = SFT_TRIVIAL(text, pattern, occurrencesCount);
            auto end = high_resolution_clock::now();
            auto durationTrivial = duration_cast<microseconds>(end - start).count();
            std::cout << "Trivial Time: " << durationTrivial / 1000000.0 << " seconds\n";

            delete[] occurrences;

            start = high_resolution_clock::now();
            occurrences = SFT_KMP(text, pattern, occurrencesCount);
            end = high_resolution_clock::now();
            auto durationKMP = duration_cast<microseconds>(end - start).count();
            std::cout << "KMP Time: " << durationKMP / 1000000.0 << " seconds\n";

            delete[] occurrences;
        }
    }
    else if (experimentChoice == 2) {
        std::cout << "Running Experiment 2: A={a,b}, Y=(a)^m, X with random letters, m=1,...,106+1 with step 10^4\n";
        for (int m = 1; m <= 1000000; m += 10000) {
            std::string text = generateRandomString(1000000 + 1, 2);
            std::string pattern(m, 'a');  // Y=(a)^m

            std::cout << "m=" << m << ": Working...\n";
            int occurrencesCount;
            auto start = high_resolution_clock::now();
            int* occurrences = SFT_TRIVIAL(text, pattern, occurrencesCount);
            auto end = high_resolution_clock::now();
            auto durationTrivial = duration_cast<microseconds>(end - start).count();
            std::cout << "Trivial Time: " << durationTrivial / 1000000.0 << " seconds\n";

            delete[] occurrences;

            start = high_resolution_clock::now();
            occurrences = SFT_KMP(text, pattern, occurrencesCount);
            end = high_resolution_clock::now();
            auto durationKMP = duration_cast<microseconds>(end - start).count();
            std::cout << "KMP Time: " << durationKMP / 1000000.0 << " seconds\n";

            delete[] occurrences;
        }
    }
    else if (experimentChoice == 3) {
        std::cout << "Running Experiment 3: Y=aaaaa, X=(aaaaab)^h, h=1,...,106+1 with step 10^4\n";
        for (int h = 1; h <= 1000000; h += 10000) {
            std::string text = generateRandomString(1000000 + 1, 2);
            std::string pattern = "aaaaa";  // Y=aaaaa

            std::cout << "h=" << h << ": Working...\n";
            int occurrencesCount;
            auto start = high_resolution_clock::now();
            int* occurrences = SFT_TRIVIAL(text, pattern, occurrencesCount);
            auto end = high_resolution_clock::now();
            auto durationTrivial = duration_cast<microseconds>(end - start).count();
            std::cout << "Trivial Time: " << durationTrivial / 1000000.0 << " seconds\n";

            delete[] occurrences;

            start = high_resolution_clock::now();
            occurrences = SFT_KMP(text, pattern, occurrencesCount);
            end = high_resolution_clock::now();
            auto durationKMP = duration_cast<microseconds>(end - start).count();
            std::cout << "KMP Time: " << durationKMP / 1000000.0 << " seconds\n";

            delete[] occurrences;
        }
    }
    else {
        std::cout << "Invalid experiment choice!\n";
    }
}

int main() {
    std::cout << "Select an option:\n";
    std::cout << "1. Run predefined experiments\n";
    std::cout << "2. Enter custom data\n";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        std::cout << "Select an experiment to run:\n";
        std::cout << "1. Y=(ab)^k, X=(ab)^1000*k\n";
        std::cout << "2. A={a,b}, Y=(a)^m, X with random letters\n";
        std::cout << "3. Y=aaaaa, X=(aaaaab)^h\n";
        int experimentChoice;
        std::cin >> experimentChoice;
        performExperiment(experimentChoice);
    }
    else if (choice == 2) {
        std::cout << "Enter text (X): ";
        std::string text;
        std::cin >> text;

        std::cout << "Enter pattern (Y): ";
        std::string pattern;
        std::cin >> pattern;

        // Measure Trivial Algorithm time
        std::cout << "Working...\n";
        int occurrencesCount;
        auto start = std::chrono::high_resolution_clock::now();
        int* occurrences = SFT_TRIVIAL(text, pattern, occurrencesCount);
        auto end = std::chrono::high_resolution_clock::now();
        auto durationTrivial = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "Trivial Algorithm Time: " << durationTrivial / 1000000.0 << " seconds\n";
        delete[] occurrences;

        // Measure KMP Algorithm time
        start = std::chrono::high_resolution_clock::now();
        occurrences = SFT_KMP(text, pattern, occurrencesCount);
        end = std::chrono::high_resolution_clock::now();
        auto durationKMP = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "KMP Algorithm Time: " << durationKMP / 1000000.0 << " seconds\n";
        delete[] occurrences;
    }

    return 0;
}
