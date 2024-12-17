#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>

// Функция для генерации случайной строки
std::string generate_random_string(size_t length, const std::string& alphabet) {
    std::string result;
    result.reserve(length);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, alphabet.size() - 1);

    for (size_t i = 0; i < length; ++i) {
        result += alphabet[dist(gen)];
    }

    return result;
}

// Функция для генерации строки по шаблону
std::string generate_pattern_string(const std::string& base, size_t k) {
    std::string result;
    result.reserve(base.size() * k);
    for (size_t i = 0; i < k; ++i) {
        result += base;
    }
    return result;
}

void SFT_TRIVIAL(const std::string& X, const std::string& Y) {
    size_t m = X.size();
    size_t n = Y.size();
    std::vector<int> f(m, -1);

    for (size_t i = 0; i <= m - n; ++i) {
        size_t s = 0;
        while (s < n && Y[s] == X[i + s]) {
            ++s;
        }

        if (s == n) {
            f[i] = i;
        }
    }
}

// Построение префикс-функции для алгоритма Кнута-Морриса-Пратта
void KMP(const std::string& Y, size_t n, std::vector<int>& fY) {
    size_t i = 1;
    fY[0] = 0;
    while (i < n) {
        int j = fY[i - 1];
        while (j > 0 && Y[j] != Y[i]) {
            j = fY[j - 1];
        }
        if (Y[j] == Y[i]) {
            fY[i] = j + 1;
        }
        else {
            fY[i] = 0;
        }
        i++;
    }
}

void SFT_KMP(const std::string& X, const std::string& Y) {
    size_t m = X.size();
    size_t n = Y.size();
    std::vector<int> fY(n, 0);
    KMP(Y, n, fY);

    std::vector<int> f(m, -1);

    size_t i = 0;
    size_t j = 0;
    while (i < m) {
        while (j > 0 && Y[j] != X[i]) {
            j = fY[j - 1];
        }

        if (Y[j] == X[i]) {
            j++;
        }

        if (j == n) {
            f[i - n + 1] = i - n + 1;
            j = fY[j - 1];
        }
        i++;
    }
}

std::vector<int> rabinKarp(const std::string& text, const std::string& pattern) {
    std::vector<int> positions;
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

template<typename Func>
double measure_time(Func func, const std::string& X, const std::string& Y) {
    const size_t iterations = 1;
    double total_duration = 0;

    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        func(X, Y);  // Передаем только X и Y
        auto end = std::chrono::high_resolution_clock::now();
        total_duration += std::chrono::duration<double>(end - start).count();
    }

    return total_duration / iterations;
}

void write_to_file(const std::string& filename, const std::string& X, const std::string& Y) {
    std::ofstream out(filename);
    if (out.is_open()) {
        out << "X: " << X << "\n";
        out << "Y: " << Y << "\n";
        out.close();
    }
    else {
        std::cerr << "Unable to open file for writing.\n";
    }
}

std::string choose_generation_method(const std::string& alphabet, const std::string& prompt) {
    int choice;
    std::cout << prompt;
    std::cin >> choice;

    size_t length;
    std::string result;

    if (choice == 1) {
        std::cout << "Enter string: ";
        std::cin >> result;
    }
    else if (choice == 2) {
        std::cout << "Enter length of the string: ";
        std::cin >> length;
        result = generate_random_string(length, alphabet);
    }
    else if (choice == 3) {
        std::string pattern;
        size_t k;
        std::cout << "Enter pattern for string: ";
        std::cin >> pattern;
        std::cout << "Enter number of repetitions (k): ";
        std::cin >> k;
        result = generate_pattern_string(pattern, k);
    }

    return result;
}

void write_experiment_to_file(const std::string& filename, const std::vector<double>& T1_values, const std::vector<double>& T2_values, const std::vector<double>& T3_values) {
    std::ofstream out(filename);
    if (out.is_open()) {
        out << "k/T1 (Trivial Algorithm)\tT2 (KMP Algorithm)\tT3 (RabinKarp)\n";
        for (size_t i = 0; i < T1_values.size(); ++i) {
            out << i + 1 << "\t" << T1_values[i] << "\t" << T2_values[i] << "\t" << T3_values[i] << "\n";
        }
        out.close();
    }
    else {
        std::cerr << "Unable to open file for writing: " << filename << "\n";
    }
}

void experiment_4_1() {
    std::vector<double> T1_values;
    std::vector<double> T2_values;
    std::vector<double> T3_values;
    for (size_t k = 1; k <= 1001; k += 10) {
        std::string Y = generate_pattern_string("ab", k);
        std::string X = generate_pattern_string("ab", 1000 * k);

        double time_trivial = measure_time(SFT_TRIVIAL, X, Y);
        double time_kmp = measure_time(SFT_KMP, X, Y);
        double time_karp = measure_time(rabinKarp, X, Y);

        T1_values.push_back(time_trivial);
        T2_values.push_back(time_kmp);
        T3_values.push_back(time_karp);

        std::cout << "k = " << k << ", T1 = " << time_trivial << "s, T2 = " << time_kmp << "s, T3 = " << time_karp << "s\n";
    }

    write_experiment_to_file("experiment_4_1_results.txt", T1_values, T2_values, T3_values);
}

void experiment_4_2() {
    std::string alphabet = "ab";
    std::vector<double> T1_values;
    std::vector<double> T2_values;
    std::vector<double> T3_values;

    for (size_t m = 1; m <= 1000000 + 1; m += 10000) {
        std::string Y = generate_pattern_string("a", m);
        std::string X = generate_random_string(1000000 + 1, alphabet);

        double time_trivial = measure_time(SFT_TRIVIAL, X, Y);
        double time_kmp = measure_time(SFT_KMP, X, Y);
        double time_karp = measure_time(rabinKarp, X, Y);

        T1_values.push_back(time_trivial);
        T2_values.push_back(time_kmp);
        T3_values.push_back(time_karp);

        std::cout << "m = " << m << ", T1 = " << time_trivial << "s, T2 = " << time_kmp << "s, T3 = " << time_karp << "s\n";
    }

    write_experiment_to_file("experiment_4_2_results.txt", T1_values, T2_values, T3_values);
}

void experiment_4_3() {
    std::vector<double> T1_values;
    std::vector<double> T2_values;
    std::vector<double> T3_values;

    for (size_t h = 1; h <= 1000000 + 1; h += 10000) {
        std::string Y = "aaaaa";
        std::string X = generate_pattern_string("aaaaab", h);

        double time_trivial = measure_time(SFT_TRIVIAL, X, Y);
        double time_kmp = measure_time(SFT_KMP, X, Y);
        double time_karp = measure_time(rabinKarp, X, Y);

        T1_values.push_back(time_trivial);
        T2_values.push_back(time_kmp);
        T3_values.push_back(time_karp);

        std::cout << "h = " << h << ", T1 = " << time_trivial << "s, T2 = " << time_kmp << "s, T3 = " << time_karp << "s\n";
    }

    write_experiment_to_file("experiment_4_3_results.txt", T1_values, T2_values, T3_values);
}

void run_experiments() {
    std::cout << "Running experiment 4.1\n";
    experiment_4_1();

    std::cout << "Running experiment 4.2\n";
    experiment_4_2();

    std::cout << "Running experiment 4.3\n";
    experiment_4_3();
}

int main() {
    int num;
    std::cout << "Enter num 1. Run experiment 2. Create experiment\n";
    std::cin >> num;
    if (num == 1) { run_experiments(); }
    if (num == 2) {
        size_t alphabet_size;
        std::cout << "Enter the size of the alphabet: ";
        std::cin >> alphabet_size;

        std::string alphabet;
        for (size_t i = 0; i < alphabet_size; ++i) {
            alphabet += char('a' + i);
        }

        std::string X = choose_generation_method(alphabet, "Choose how to generate X (1 - direct input, 2 - random generation, 3 - pattern generation): ");
        std::string Y = choose_generation_method(alphabet, "Choose how to generate Y (1 - direct input, 2 - random generation, 3 - pattern generation): ");

        write_to_file("strings.txt", X, Y);

        double time_trivial = measure_time(SFT_TRIVIAL, X, Y);
        double time_kmp = measure_time(SFT_KMP, X, Y);
        double time_karp = measure_time(rabinKarp, X, Y);

        std::cout << "Time for trivial algorithm: " << time_trivial << " seconds\n";
        std::cout << "Time for KMP algorithm: " << time_kmp << " seconds\n";
        std::cout << "Time for RabinKarp algorithm: " << time_karp << " seconds\n";
    }
    else {
        std::cout << "Error";
        return 0;
    }
    return 0;
}
