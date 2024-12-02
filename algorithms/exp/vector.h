#ifndef INCLUDE_VECTOR_H_
#define INCLUDE_VECTOR_H_

#include <iostream>
#include <stdexcept>
#include <cassert>
#include <algorithm>

const int MAX_VECTOR_SIZE = 100000000;

template<typename T>
class TVector {
protected:
    size_t _size;
    size_t _capacity;
    T* pMem;

public:
    // Конструктор по умолчанию (с размером 1)
    TVector(size_t size = 1) : _size(size), _capacity(size) {
        if (size == 0)
            throw std::length_error("Vector size should be greater than zero");
        if (size > MAX_VECTOR_SIZE)
            throw std::length_error("Vector size cannot be greater than MAX_VECTOR_SIZE");

        pMem = new(std::nothrow) T[size];
        if (!pMem)
            throw std::bad_alloc();  // Защита от ошибок выделения памяти
    }

    // Конструктор, инициализирующий вектор из массива
    TVector(const T* data, size_t size) : _size(size), _capacity(size) {
        assert(data != nullptr && "TVector constructor requires non-nullptr argument.");

        pMem = new(std::nothrow) T[size];
        if (!pMem)
            throw std::bad_alloc();  // Защита от ошибок выделения памяти
        std::copy(data, data + size, pMem);
    }

    // Конструктор копирования
    TVector(const TVector& v) : _size(v._size), _capacity(v._capacity), pMem(new(std::nothrow) T[v._capacity]) {
        if (!pMem)
            throw std::bad_alloc();  // Защита от ошибок выделения памяти
        std::copy(v.pMem, v.pMem + v._size, pMem);
    }

    // Конструктор перемещения
    TVector(TVector&& v) noexcept : _size(0), _capacity(0), pMem(nullptr) {
        swap(*this, v);
    }

    // Конструктор, инициализирующий вектор с размером и значением для всех элементов
    TVector(size_t size, const T& value) : _size(size), _capacity(size) {
        if (size == 0)
            throw std::length_error("Vector size should be greater than zero");
        if (size > MAX_VECTOR_SIZE)
            throw std::length_error("Vector size cannot be greater than MAX_VECTOR_SIZE");

        pMem = new(std::nothrow) T[size];
        if (!pMem)
            throw std::bad_alloc();  // Защита от ошибок выделения памяти
        std::fill(pMem, pMem + size, value);
    }

    // Деструктор
    ~TVector() {
        delete[] pMem;
    }

    // Оператор присваивания
    TVector& operator=(TVector v) {
        swap(*this, v);
        return *this;
    }

    // Возвращает размер вектора
    size_t size() const noexcept {
        return _size;
    }

    // Возвращает емкость вектора
    size_t capacity() const noexcept {
        return _capacity;
    }

    // Добавляет элемент в конец вектора
    void push_back(const T& value) {
        if (_size == _capacity) {
            // Увеличиваем емкость вектора
            if (_capacity == 0) {
                _capacity = 1;
            }
            else {
                // Проверка на переполнение при умножении
                if (_capacity > MAX_VECTOR_SIZE / 2) {
                    throw std::length_error("Exceeded maximum allowable capacity");
                }
                _capacity = _capacity * 2;  // Увеличиваем емкость
            }

            T* newMem = new(std::nothrow) T[_capacity];
            if (!newMem) {
                throw std::bad_alloc();  // Защита от ошибок выделения памяти
            }

            std::copy(pMem, pMem + _size, newMem); // Копируем старые элементы в новый массив
            delete[] pMem;  // Освобождаем старую память
            pMem = newMem;  // Устанавливаем новый указатель
        }

        pMem[_size++] = value;  // Добавляем новый элемент
    }



    // Оператор индексации для доступа к элементам
    T& operator[](size_t ind) {
        if (ind >= _size) {
            throw std::out_of_range("Index out of range");
        }
        return pMem[ind];
    }

    // Оператор индексации (константный) для доступа к элементам
    const T& operator[](size_t ind) const {
        if (ind >= _size) {
            throw std::out_of_range("Index out of range");
        }
        return pMem[ind];
    }

    // Метод для получения элемента по индексу с проверкой
    T& at(size_t ind) {
        if (ind >= _size) {
            throw std::out_of_range("Index out of range");
        }
        return pMem[ind];
    }

    // Метод для получения элемента по индексу с проверкой (константный)
    const T& at(size_t ind) const {
        if (ind >= _size) {
            throw std::out_of_range("Index out of range");
        }
        return pMem[ind];
    }

    // Оператор равенства
    bool operator==(const TVector& v) const noexcept {
        if (_size != v._size) {
            return false;
        }
        for (size_t i = 0; i < _size; ++i) {
            if (pMem[i] != v.pMem[i]) {
                return false;
            }
        }
        return true;
    }

    // Оператор неравенства
    bool operator!=(const TVector& v) const noexcept {
        return !(*this == v);
    }

    // Оператор сложения с числом
    TVector operator+(T val) const {
        TVector res(*this);
        for (size_t i = 0; i < _size; ++i) {
            res.pMem[i] += val;
        }
        return res;
    }

    // Оператор вычитания с числом
    TVector operator-(T val) const {
        TVector res(*this);
        for (size_t i = 0; i < _size; ++i) {
            res.pMem[i] -= val;
        }
        return res;
    }

    // Оператор умножения на число
    TVector operator*(T val) const {
        TVector res(*this);
        for (size_t i = 0; i < _size; ++i) {
            res.pMem[i] *= val;
        }
        return res;
    }

    // Оператор скалярного произведения
    T operator*(const TVector& v) const {
        if (_size != v._size) {
            throw std::length_error("Dimension mismatch");
        }
        T dotProduct = 0;
        for (size_t i = 0; i < _size; ++i) {
            dotProduct += pMem[i] * v.pMem[i];
        }
        return dotProduct;
    }

    // Оператор сложения двух векторов
    TVector operator+(const TVector& v) const {
        if (_size != v._size) {
            throw std::length_error("Dimension mismatch");
        }
        TVector res(*this);
        for (size_t i = 0; i < _size; ++i) {
            res.pMem[i] += v.pMem[i];
        }
        return res;
    }

    // Оператор вычитания двух векторов
    TVector operator-(const TVector& v) const {
        if (_size != v._size) {
            throw std::length_error("Dimension mismatch");
        }
        TVector res(*this);
        for (size_t i = 0; i < _size; ++i) {
            res.pMem[i] -= v.pMem[i];
        }
        return res;
    }

    // Обмен данными между векторами
    friend void swap(TVector& lhs, TVector& rhs) noexcept {
        std::swap(lhs._size, rhs._size);
        std::swap(lhs._capacity, rhs._capacity);
        std::swap(lhs.pMem, rhs.pMem);
    }

    // Ввод вектора с потока
    friend std::istream& operator>>(std::istream& istr, TVector& v) {
        for (size_t i = 0; i < v._size; ++i) {
            istr >> v.pMem[i];
        }
        return istr;
    }

    // Вывод вектора в поток
    friend std::ostream& operator<<(std::ostream& ostr, const TVector& v) {
        for (size_t i = 0; i < v._size; ++i) {
            ostr << v.pMem[i] << ' ';
        }
        return ostr;
    }

    // Метод для начала диапазона
    T* begin() {
        return pMem;
    }

    // Метод для конца диапазона
    T* end() {
        return pMem + _size;
    }
};

#endif  // INCLUDE_VECTOR_H_
