#pragma once
#include <cassert>

auto beginVolume = 128;
auto constEnlarg = 1.2f;

template <typename Type>
class Vector
{
private:	
	int capacity;
	int length;
	Type* data;
public:
	explicit Vector() : capacity(0), length(0) {};
	explicit Vector(int size, const Type& value);
	Vector(const Vector<Type>& A);
	~Vector();

	void PushBack(const Type& item);
	void PopBack();
	void Erase(int k);
	void Reserve(int n);
	void Resize(int n, Type item = Type());
	void Clear();
	int Size() { return length; }
	int Capacity() { return capacity; }
	bool Empty() { return !length; }
	Type& operator[](int index)
	{
		assert(index >= 0 && index < length);
		return data[index];
	}
};

template<typename Type> Vector<Type>::Vector(int size, const Type& value = Type())
{
	capacity = size;
	length = size;
	data = (Type*)std::malloc(capacity * sizeof(Type));
	for (int i = 0; i < size; i++)
		new (&data[i]) Type(value);
}

template<typename Type> Vector<Type>::Vector(const Vector<Type>& A)
{
	capacity = A.capacity;
	length = A.length;
	data = (Type*)std::malloc(capacity * sizeof(Type));
	std::memcpy(data, A.data, length * sizeof(Type));
}

template<typename Type> Vector<Type>::~Vector()
{
	if (capacity)
	{
		for (int i = 0; i < length; i++)
			((Type*)&data[i])->~Type();
		std::free(data);
	}
}

template<typename Type> void Vector<Type>::PushBack(const Type& item)
{
	if (length >= capacity)
	{
		if (!capacity)
		{
			capacity = beginVolume / sizeof(Type);
			data = (Type*)std::malloc(capacity*sizeof(Type));
		}
		else
		{
			capacity *= constEnlarg;
			Type* newData = (Type*)std::malloc(capacity * sizeof(Type));
			std::memcpy(newData, data, length * sizeof(Type));
			std::free(data);
			data = newData;
		}
	}
	new (&data[length++]) Type(item);
}

template<typename Type> void Vector<Type>::PopBack()
{
	assert(this->length > 0);
	((Type*)&data[length-1])->~Type();
	--length;
}

template<typename Type> void Vector<Type>::Erase(int k)
{
	((Type*)&data[k])->~Type();
	--length;
	std::memmove((Type*)&data[k], (Type*)&data[k+1], (length-k) * sizeof(Type));	
}

template<typename Type> void Vector<Type>::Reserve(int n)
{
	if (capacity != n)
	{
		capacity = n;
		if (n < length)
		{
			for (int i = n; i < length; i++)
				((Type*)&data[i])->~Type();
			length = n;
		}
		Type* newData = (Type*)std::malloc(capacity * sizeof(Type));
		std::memcpy(newData, data, length * sizeof(Type));
		std::free(data);
		data = newData;
	}
}

template<typename Type> void Vector<Type>::Resize(int n, Type item = Type())
{
	if (n <= length)	
	{
		for (int i = n; i < length; i++)
			((Type*)&data[i])->~Type();
	}
	else
	{
		if (n > capacity)
		{
			capacity = n;
			Type* newData = (Type*)std::malloc(capacity * sizeof(Type));
			std::memcpy(newData, data, length * sizeof(Type));
			std::free(data);
			data = newData;
		}
		for (int i = length; i < n; i++)
			new (&data[i]) Type(item);
	}
	length = n;
}

template<typename Type> void Vector<Type>::Clear()
{
	for (int i=0; i<length; i++)
		((Type*)&data[i])->~Type();
	length = 0;
}
