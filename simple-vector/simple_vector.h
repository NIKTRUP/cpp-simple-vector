#pragma once

/*
 * "хорошая реализация. Не хватает пары проверок, но закрою вам работу"
 * Спасибо). По тестам всё хорошо? А что бы вы добавили , если бы писали этот код ?
 */



#include <cassert>
#include <initializer_list>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <utility>
#include "array_ptr.h"

class ReserveProxyObj{
public:
    //ReserveProxyObj() = default;
    ReserveProxyObj(size_t capacity_to_reserve): reserve_(capacity_to_reserve){}

    size_t Capacity() const {
        return reserve_;
    }

private:
    size_t reserve_ = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    //SimpleVector() noexcept = default;
    SimpleVector() noexcept: size_(0), capacity_(0) {};

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) : ptr_(size), size_(size), capacity_(size) {
        std::fill(begin(), end(), Type());
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) : ptr_(size), size_(size), capacity_(size) {
            std::fill(begin(), end(), value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) : ptr_(init.size()), size_(init.size()), capacity_(init.size()) {
        std::copy(init.begin(), init.end(), begin());
    }

    SimpleVector(const SimpleVector& other) {
        SimpleVector temp(other.GetCapacity());
        std::copy(other.cbegin(), other.cend(), temp.begin());
        temp.size_ = other.GetSize();
        temp.capacity_ = other.GetCapacity();
        swap(temp);
    }

    SimpleVector(const ReserveProxyObj& reserve_obj) :
        ptr_(reserve_obj.Capacity()),
        size_(0),
        capacity_(reserve_obj.Capacity()) {
    }

    SimpleVector(SimpleVector&& other):
        ptr_(std::move(other.ptr_)),
        size_(std::exchange(other.size_, 0)),
        capacity_(std::exchange(other.capacity_, 0)) {}


    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector<Type> tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs){
        if (this != &rhs){
            SimpleVector tmp(std::move(rhs));
            swap(tmp);
        }
        return *this;
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if(index >= size_){
            throw std::out_of_range("out of range");
        }
        return ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if(index >= size_){
            throw std::out_of_range("out of range");
        }
        return ptr_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    /*
     std::fill пытается присвоить значение, по сути выполнив копирование переданного вами элемента.
     Если элемент не копируемый, как в тесте, то ничего не получится
    */
    void Fill(Iterator begin, Iterator end){
        for(auto it_ = begin, end_ = end; it_ != end_; ++it_){
             *it_ = Type{};
        }
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Reserve(size_t new_capacity){
        if(new_capacity > capacity_){
            ArrayPtr<Type> new_items(new_capacity);
            std::move(begin(), end(), new_items.Get());
            ptr_.swap(new_items);
            capacity_ = new_capacity;
        }
    }

    void Resize(size_t new_size)
     {
        if (new_size > GetSize()) {
            if (new_size > GetCapacity()) {
                Reserve(std::max(new_size, 2 * capacity_));
                Fill(ptr_.Get() + GetSize(), ptr_.Get() + GetCapacity());
            }
            else {
                Fill(ptr_.Get() + GetSize(), ptr_.Get() + GetCapacity());
            }
        }
        size_ = new_size;
        //std::fill(begin() + GetSize(), ptr_.Get() + GetCapacity(), Type());
     }


    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type&& item){
         size_t init = 1;
         if (size_ < capacity_){
             ptr_[size_] = std::move(item);
         }else {
             Reserve(std::max(init, 2 * capacity_));
             ptr_[size_] = std::move(item);
         }
         ++size_;
     }

     void PushBack(const Type& item){
         size_t init = 1;
         if (size_ < capacity_){
             ptr_[size_] = item;
         } else {
             Reserve(std::max(init, 2 * capacity_));
             ptr_[size_] = item;
         }
         ++size_;
     }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
     Iterator Insert(ConstIterator pos, Type&& value){
         assert(pos <= end() && pos >= begin());
         size_t number_pos = Move(pos);
         ptr_[number_pos] = std::move(value);
         return &ptr_[number_pos];
     }

     Iterator Insert(ConstIterator pos, const Type& value){
         assert(pos <= end() && pos >= begin());
         size_t number_pos = Move(pos);
         ptr_[number_pos] = value;
         return &ptr_[number_pos];
     }


    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (!IsEmpty()) {
            --size_;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos){
        assert(pos < end() && pos >= begin());
        Iterator it = &ptr_[std::distance<ConstIterator>(cbegin(), pos)];
        std::move(it + 1, end(), it);
        --size_;
        return it;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        ptr_.swap(other.ptr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    void swap(SimpleVector&& other) noexcept {
        ptr_.swap(other.ptr_Z);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        other.Clear();
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return ptr_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return ptr_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
       return ptr_.Get(); // return cbegin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
       return ptr_.Get() + size_; //return cend();
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return ptr_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return ptr_.Get() + size_;
    }
private:
    size_t Move(ConstIterator pos){
        size_t number_pos = std::distance<ConstIterator>(cbegin(), pos);
        if (size_ < capacity_){
            if (pos == end()){
                assert(pos == end());
            }
            std::move_backward(&ptr_[number_pos], end(), &ptr_[size_ + 1]);
        } else{
            if (capacity_ == 0){
                Reserve(1);
            }else {
                Reserve(2 * capacity_);
                std::move_backward(&ptr_[number_pos], end(), &ptr_[size_ + 1]);
            }
        }
        ++size_;
        return number_pos;
    }

    ArrayPtr<Type> ptr_;
    size_t size_;
    size_t capacity_;
};


template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs.GetSize() == rhs.GetSize()) && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(
        lhs.begin(), lhs.end(),
        rhs.begin(), rhs.end()
    );
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}

