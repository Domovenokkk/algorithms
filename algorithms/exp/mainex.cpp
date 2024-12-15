#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>

// Функция для генерации случайной строки
std::string generate_random_string(size_t length, const std::string& alphabet) {
    std::string result;
    result.reserve(length); // Резервируем память для строки
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
    result.reserve(base.size() * k); // Резервируем память
    for (size_t i = 0; i < k; ++i) {
        result += base;
    }
    return result;
}

// Тривиальный алгоритм поиска подстроки
void SFT_TRIVIAL(const std::string& X, size_t m, const std::string& Y, size_t n, std::vector<int>& f) {
    f.resize(m, -1);  // Инициализация вектора с значениями -1

    for (size_t i = 0; i <= m - n; ++i) { // Проходим по всем возможным сдвигам
        size_t s = 0;
        while (s < n && Y[s] == X[i + s]) { // Сравниваем символы
            ++s;
        }

        if (s == n) {
            f[i] = i; // Совпадение найдено, сохраняем индекс начала
        }
    }
}

// Построение префикс-функции для алгоритма Кнута-Морриса-Пратта
void KMP(const std::string& Y, size_t n, std::vector<int>& fY) {
    size_t i = 1;
    fY[0] = 0; // Начальная префикс-функция равна 0
    while (i < n) {
        int j = fY[i - 1]; // j = fY[i]
        while (j > 0 && Y[j] != Y[i]) { // Поиск с использованием префикс-функции
            j = fY[j - 1];
        }
        if (Y[j] == Y[i]) {
            fY[i] = j + 1; // Префикс совпал
        }
        else {
            fY[i] = 0; // Префикс не совпал
        }
        i++;
    }
}

// Алгоритм Кнута-Морриса-Пратта для поиска подстроки
void SFT_KMP(const std::string& X, size_t m, const std::string& Y, size_t n, std::vector<int>& f) {
    std::vector<int> fY(n, 0);  // Вектор префикс-функций для Y
    KMP(Y, n, fY);  // Строим префикс-функцию для Y

    f.resize(m, -1);  // Инициализируем вектор с -1

    size_t i = 0;  // Индекс для строки X
    size_t j = 0;  // Индекс для строки Y
    while (i < m) {
        // Ищем совпадения в строках X и Y
        while (j > 0 && Y[j] != X[i]) {
            j = fY[j - 1];  // Ищем по префикс-функции
        }

        if (Y[j] == X[i]) {
            j++;  // Префикс совпал
        }

        if (j == n) {  // Полное совпадение найдено
            f[i - n + 1] = i - n + 1;  // Сохраняем индекс начала совпадения
            j = fY[j - 1];  // Продолжаем искать дальше
        }
        i++;
    }
}

// Функция для замера времени работы алгоритма
template<typename Func>
double measure_time(Func func, const std::string& X, const std::string& Y) {
    std::vector<int> f;  // Вектор для хранения результатов (например, функции префиксов)
    const size_t iterations = 1; // Количество повторений для усреднения
    double total_duration = 0;

    for (size_t i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        func(X, X.size(), Y, Y.size(), f); // Вызов алгоритма поиска
        auto end = std::chrono::high_resolution_clock::now();
        total_duration += std::chrono::duration<double>(end - start).count();
    }

    return total_duration / iterations; // Усреднённое время выполнения
}

// Функция для записи строк в файл
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

// Функция для выбора способа генерации строки
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

void write_experiment_to_file(const std::string& filename, const std::vector<double>& T1_values, const std::vector<double>& T2_values) {
    std::ofstream out(filename);
    if (out.is_open()) {
        out << "k/T1 (Trivial Algorithm)\tT2 (KMP Algorithm)\n";
        for (size_t i = 0; i < T1_values.size(); ++i) {
            out << i + 1 << "\t" << T1_values[i] << "\t" << T2_values[i] << "\n";
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

    for (size_t k = 1; k <= 1001; k += 10) {
        std::string Y = generate_pattern_string("ab", k);
        std::string X = generate_pattern_string("ab", 1000 * k);

        double time_trivial = measure_time(SFT_TRIVIAL, X, Y);
        double time_kmp = measure_time(SFT_KMP, X, Y);

        T1_values.push_back(time_trivial);
        T2_values.push_back(time_kmp);

        std::cout << "k = " << k << ", T1 = " << time_trivial << "s, T2 = " << time_kmp << "s\n";
    }

    write_experiment_to_file("experiment_4_1_results.txt", T1_values, T2_values);
}

void experiment_4_2() {
    std::string alphabet = "ab";
    std::vector<double> T1_values;
    std::vector<double> T2_values;

    for (size_t m = 1; m <= 1000000 + 1; m += 10000) {
        std::string Y = generate_pattern_string("a", m);
        std::string X = generate_random_string(1000000 + 1, alphabet);

        double time_trivial = measure_time(SFT_TRIVIAL, X, Y);
        double time_kmp = measure_time(SFT_KMP, X, Y);

        T1_values.push_back(time_trivial);
        T2_values.push_back(time_kmp);

        std::cout << "m = " << m << ", T1 = " << time_trivial << "s, T2 = " << time_kmp << "s\n";
    }

    write_experiment_to_file("experiment_4_2_results.txt", T1_values, T2_values);
}

void experiment_4_3() {
    std::vector<double> T1_values;
    std::vector<double> T2_values;

    for (size_t h = 1; h <= 1000000 + 1; h += 10000) {
        std::string Y = "aaaaa";
        std::string X = generate_pattern_string("aaaaab", h);

        double time_trivial = measure_time(SFT_TRIVIAL, X, Y);
        double time_kmp = measure_time(SFT_KMP, X, Y);

        T1_values.push_back(time_trivial);
        T2_values.push_back(time_kmp);

        std::cout << "h = " << h << ", T1 = " << time_trivial << "s, T2 = " << time_kmp << "s\n";
    }

    write_experiment_to_file("experiment_4_3_results.txt", T1_values, T2_values);
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

        std::cout << "Time for trivial algorithm: " << time_trivial << " seconds\n";
        std::cout << "Time for KMP algorithm: " << time_kmp << " seconds\n";
    }
    else {
        std::cout << "Error";
        return 0;
    }
    return 0;
}
