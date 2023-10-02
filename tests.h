#pragma once
#include <cassert>
#include <stdexcept>
#include <numeric>
#include "simple_vector.h"

using namespace std::literals::string_literals;


template <typename Function>
void RunTestImpl(Function function,const std::string& func) {
    function();
    std::cerr << func << " OK"s << std::endl;
}

#define RUN_TEST(func)  RunTestImpl( (func) , #func )



void TestReserveConstructor() {
    SimpleVector<int> v(Reserve(5));
    assert(v.GetCapacity() == 5);
    assert(v.IsEmpty());
}

void TestReserveMethod() {
    SimpleVector<int> v;
    // зарезервируем 5 мест в векторе
    v.Reserve(5);
    assert(v.GetCapacity() == 5);
    assert(v.IsEmpty());

    // попытаемся уменьшить capacity до 1
    v.Reserve(1);
    // capacity должно остаться прежним
    assert(v.GetCapacity() == 5);
    // поместим 10 элементов в вектор
    for (int i = 0; i < 10; ++i) {
        v.PushBack(i);
    }
    assert(v.GetSize() == 10);
    // увеличим capacity до 100
    v.Reserve(100);
    // проверим, что размер не поменялся
    assert(v.GetSize() == 10);
    assert(v.GetCapacity() == 100);
    // проверим, что элементы на месте
    for (int i = 0; i < 10; ++i) {
        assert(v[i] == i);
    }
}

void TestConctructors(){
    // Инициализация конструктором по умолчанию
    {
        SimpleVector<int> v;
        assert(v.GetSize() == 0u);
        assert(v.IsEmpty());
        assert(v.GetCapacity() == 0u);
    }

    // Инициализация вектора указанного размера
    {
        SimpleVector<int> v(5);
        assert(v.GetSize() == 5u);
        assert(v.GetCapacity() == 5u);
        assert(!v.IsEmpty());
        for (size_t i = 0; i < v.GetSize(); ++i) {
            assert(v[i] == 0);
        }
    }

    // Инициализация вектора, заполненного заданным значением
    {
        SimpleVector<int> v(3, 42);
        assert(v.GetSize() == 3);
        assert(v.GetCapacity() == 3);
        for (size_t i = 0; i < v.GetSize(); ++i) {
            assert(v[i] == 42);
        }
    }

    // Инициализация вектора при помощи initializer_list
    {
        SimpleVector<int> v{1, 2, 3};
        assert(v.GetSize() == 3);
        assert(v.GetCapacity() == 3);
        assert(v[2] == 3);
    }

    // Конструктор копирования
    {
        SimpleVector<int> v{1, 2, 3, 4};
        assert(v.GetSize() == 4 && v.GetCapacity() == 4);
        SimpleVector<int> v2(v);
        assert(v2.GetSize() == 4 && v2.GetCapacity() == 4);
        assert(v.GetSize() == 4 && v2.GetCapacity() == 4);
        for(int i = 0 ; i < (int)v.GetSize(); ++i){
             assert(v[i] == (i+1));
             assert(v2[i] == (i+1));
        }
    }
    // Конструктор перемещения
    {
        SimpleVector<int> v{1, 2, 3, 4};
        assert(v.GetSize() == 4 && v.GetCapacity() == 4);
        SimpleVector<int> v2(std::move(v));
        assert(v2.GetSize() == 4 && v2.GetCapacity() == 4);
        for(int i = 0 ; i < (int)v2.GetSize(); ++i){
             assert(v2[i] == (i+1));
        }
    }
}

void TestResize(){
    // Изменение размера
    {
        SimpleVector<int> v(3);
        v[2] = 17;
        v.Resize(7);
        assert(v.GetSize() == 7);
        assert(v.GetCapacity() >= v.GetSize());
        assert(v[2] == 17);
        assert(v[3] == 0);
    }
    {
        SimpleVector<int> v(3);
        v[0] = 42;
        v[1] = 55;
        const size_t old_capacity = v.GetCapacity();
        v.Resize(2);
        assert(v.GetSize() == 2);
        assert(v.GetCapacity() == old_capacity);
        assert(v[0] == 42);
        assert(v[1] == 55);
    }
    {
        const size_t old_size = 3;
        SimpleVector<int> v(3);
        v.Resize(old_size + 5);
        v[3] = 42;
        v.Resize(old_size);
        v.Resize(old_size + 2);
        assert(v[3] == 0);
    }
}

void TestClear(){
    // Очистка вектора
    {
        SimpleVector<int> v(10);
        const size_t old_capacity = v.GetCapacity();
        v.Clear();
        assert(v.GetSize() == 0);
        assert(v.GetCapacity() == old_capacity);
    }
}

void TestIterator(){
    // Итерирование по SimpleVector
    {
        // Пустой вектор
        {
            SimpleVector<int> v;
            assert(v.begin() == nullptr);
            assert(v.end() == nullptr);
        }

        // Непустой вектор
        {
            SimpleVector<int> v(10, 42);
            assert(v.begin());
            assert(*v.begin() == 42);
            assert(v.end() == v.begin() + v.GetSize());
        }
        // Итерирование по вектору
        {
            SimpleVector<int> v(10, 42);
            for(auto& el: v){
                assert(el == 42);
            }

            for(auto el: v){
                assert(el == 42);
            }

            for(const auto& el: v){
                assert(el == 42);
            }

            for(const auto el: v){
                assert(el == 42);
            }
        }
    }
}

void TestAt(){
    // Доступ к элементам при помощи At
    {
        SimpleVector<int> v(3);
        assert(&v.At(2) == &v[2]);
        try {
            v.At(3);
            assert(false);  // Ожидается выбрасывание исключения
        } catch (const std::out_of_range&) {
        } catch (...) {
            assert(false);  // Не ожидается исключение, отличное от out_of_range
        }
    }

}

inline void TestOperations() {
    // PushBack
    {
        SimpleVector<int> v(1);
        v.PushBack(42);
        assert(v.GetSize() == 2);
        assert(v.GetCapacity() >= v.GetSize());
        assert(v[0] == 0);
        assert(v[1] == 42);
    }

    // PushBack
    {
        SimpleVector<int> v(1);
        v.PushBack(std::move(42));
        assert(v.GetSize() == 2);
        assert(v.GetCapacity() >= v.GetSize());
        assert(v[0] == 0);
        assert(v[1] == 42);
    }

    // Если хватает места, PushBack не увеличивает Capacity
    {
        SimpleVector<int> v(2);
        v.Resize(1);
        const size_t old_capacity = v.GetCapacity();
        v.PushBack(123);
        assert(v.GetSize() == 2);
        assert(v.GetCapacity() == old_capacity);
    }

    // PopBack
    {
        SimpleVector<int> v{0, 1, 2, 3};
        const size_t old_capacity = v.GetCapacity();
        const auto old_begin = v.begin();
        v.PopBack();
        assert(v.GetCapacity() == old_capacity);
        assert(v.begin() == old_begin);
        assert((v == SimpleVector<int>{0, 1, 2}));
    }

    // Конструктор копирования
    {
        SimpleVector<int> numbers{1, 2};
        auto numbers_copy(numbers);
        assert(&numbers_copy[0] != &numbers[0]);
        assert(numbers_copy.GetSize() == numbers.GetSize());
        for (size_t i = 0; i < numbers.GetSize(); ++i) {
            assert(numbers_copy[i] == numbers[i]);
            assert(&numbers_copy[i] != &numbers[i]);
        }
    }

    // Сравнение
    {
        assert((SimpleVector{1, 2, 3} == SimpleVector{1, 2, 3}));
        assert((SimpleVector{1, 2, 3} != SimpleVector{1, 2, 2}));

        assert((SimpleVector{1, 2, 3} < SimpleVector{1, 2, 3, 1}));
        assert((SimpleVector{1, 2, 3} > SimpleVector{1, 2, 2, 1}));

        assert((SimpleVector{1, 2, 3} >= SimpleVector{1, 2, 3}));
        assert((SimpleVector{1, 2, 4} >= SimpleVector{1, 2, 3}));
        assert((SimpleVector{1, 2, 3} <= SimpleVector{1, 2, 3}));
        assert((SimpleVector{1, 2, 3} <= SimpleVector{1, 2, 4}));
    }

    // Обмен значений векторов
    {
        SimpleVector<int> v1{42, 666};
        SimpleVector<int> v2;
        v2.PushBack(0);
        v2.PushBack(1);
        v2.PushBack(2);
        const int* const begin1 = &v1[0];
        const int* const begin2 = &v2[0];

        const size_t capacity1 = v1.GetCapacity();
        const size_t capacity2 = v2.GetCapacity();

        const size_t size1 = v1.GetSize();
        const size_t size2 = v2.GetSize();

        static_assert(noexcept(v1.swap(v2)));
        v1.swap(v2);
        assert(&v2[0] == begin1);
        assert(&v1[0] == begin2);
        assert(v1.GetSize() == size2);
        assert(v2.GetSize() == size1);
        assert(v1.GetCapacity() == capacity2);
        assert(v2.GetCapacity() == capacity1);
    }

    // Присваивание
    {
        SimpleVector<int> src_vector{1, 2, 3, 4};
        SimpleVector<int> dst_vector{1, 2, 3, 4, 5, 6};
        dst_vector = src_vector;
        assert(dst_vector == src_vector);
    }

    // Вставка элементов
    {
        SimpleVector<int> v{1, 2, 3, 4};
        v.Insert(v.begin() + 2, 42);
        assert((v == SimpleVector<int>{1, 2, 42, 3, 4}));
    }

    // Удаление элементов
    {
        SimpleVector<int> v{1, 2, 3, 4};
        v.Erase(v.cbegin() + 2);
        assert((v == SimpleVector<int>{1, 2, 4}));
    }

    {
        SimpleVector<int> v;
        v.PushBack(42);
        assert(v.GetSize() == 1);
        assert(v.GetCapacity() >= v.GetSize());
        assert(v[0] == 42);
    }
}


class X {
public:
    X()
        : X(5) {
    }
    X(size_t num)
        : x_(num) {
    }
    X(const X& other) = delete;
    X& operator=(const X& other) = delete;
    X(X&& other) {
        x_ = std::exchange(other.x_, 0);
    }
    X& operator=(X&& other) {
        x_ = std::exchange(other.x_, 0);
        return *this;
    }
    size_t GetX() const {
        return x_;
    }

private:
    size_t x_;
};

SimpleVector<int> GenerateVector(size_t size) {
    SimpleVector<int> v(size);
    std::iota(v.begin(), v.end(), 1);
    return v;
}

void TestTemporaryObjConstructor() {
    const size_t size = 1000000;
    SimpleVector<int> moved_vector(GenerateVector(size));
    assert(moved_vector.GetSize() == size);
}

void TestTemporaryObjOperator() {
    const size_t size = 1000000;
    SimpleVector<int> moved_vector;
    assert(moved_vector.GetSize() == 0);
    moved_vector = GenerateVector(size);
    assert(moved_vector.GetSize() == size);
}

void TestNamedMoveConstructor() {
    const size_t size = 1000000;
    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector(std::move(vector_to_move));
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);
}

void TestNamedMoveOperator() {
    const size_t size = 1000000;
    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector = std::move(vector_to_move);
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);
}

void TestNoncopiableMoveConstructor() {
    const size_t size = 5;
    SimpleVector<X> vector_to_move;
    for (size_t i = 0; i < size; ++i) {
        vector_to_move.PushBack(X(i));
    }

    SimpleVector<X> moved_vector = std::move(vector_to_move);
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);

    for (size_t i = 0; i < size; ++i) {
        assert(moved_vector[i].GetX() == i);
    }
}

void TestNoncopiablePushBack() {
    const size_t size = 5;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    assert(v.GetSize() == size);

    for (size_t i = 0; i < size; ++i) {
        assert(v[i].GetX() == i);
    }
}

void TestNoncopiableInsert() {
    const size_t size = 5;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    // в начало
    v.Insert(v.begin(), X(size + 1));
    assert(v.GetSize() == size + 1);
    assert(v.begin()->GetX() == size + 1);
    // в конец
    v.Insert(v.end(), X(size + 2));
    assert(v.GetSize() == size + 2);
    assert((v.end() - 1)->GetX() == size + 2);
    // в середину
    v.Insert(v.begin() + 3, X(size + 3));
    assert(v.GetSize() == size + 3);
    assert((v.begin() + 3)->GetX() == size + 3);
}

void TestNoncopiableErase() {
    const size_t size = 3;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    auto it = v.Erase(v.begin());
    assert(it->GetX() == 1);
}

void TestSimpleVector(){
    RUN_TEST(TestTemporaryObjConstructor);
    RUN_TEST(TestTemporaryObjOperator);
    RUN_TEST(TestNamedMoveConstructor);
    RUN_TEST(TestNamedMoveOperator);
    RUN_TEST(TestNoncopiableMoveConstructor);
    RUN_TEST(TestNoncopiablePushBack);
    RUN_TEST(TestNoncopiableInsert);
    RUN_TEST(TestNoncopiableErase);
    RUN_TEST(TestClear);
    RUN_TEST(TestIterator);
    RUN_TEST(TestAt);
    RUN_TEST(TestOperations);
    RUN_TEST(TestResize);
    RUN_TEST(TestReserveConstructor);
    RUN_TEST(TestReserveMethod);
    RUN_TEST(TestConctructors);
}

