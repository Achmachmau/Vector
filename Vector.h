#pragma once
#include <cassert>

template <typename Type>
class Vector
{
private:
	int buffLen;
	int dataLen;
	Type* pData;
public:
	explicit Vector() : buffLen(0), dataLen(0), pData(nullptr) {};
	explicit Vector(int n, const Type& item = Type());
	Vector(const Vector<Type>& other);
	Vector(Vector<Type>&& tmp);
	~Vector();

	template<typename TReference>
	void PushBack(TReference&& item);
	void PopBack();
	void Erase(unsigned index);
	void Erase(unsigned first, unsigned last);
	void Reserve(int n);
	void Resize(int n, Type item = Type());
	void Clear();
	int Size() { return dataLen; }
	int Capacity() { return buffLen; }
	bool Empty() { return !dataLen; }

	Vector<Type>& operator=(const Vector<Type>& other);
	Vector<Type>& operator=(Vector<Type>&& tmp);
	Type& operator[](int index)
	{
		assert(index >= 0 && index < dataLen);
		return pData[index];
	}

	Type* begin() { return pData; }
	Type* end() { return &pData[dataLen]; }

private:
	void Swap(Vector<Type>& other);
	void AllocData();
	void Destroy();
	void EnlargBuff();
};

template<typename Type>
Vector<Type>::Vector(int n, const Type& item)
	:buffLen(n), dataLen(n), pData(nullptr)
{
	if (n)
	{
		AllocData();
		for (int i = 0; i < n; i++)
			new (&pData[i]) Type(item);
	}
}

template<typename Type>
Vector<Type>::Vector(const Vector<Type>& other)
	:buffLen(other.buffLen), dataLen(other.dataLen), pData(nullptr)
{
	if (other.pData)
	{
		AllocData();
		for (int i = 0; i < dataLen; i++)
			new (&pData[i]) Type(other.pData[i]);
	}
}

template<typename Type>
Vector<Type>::Vector(Vector<Type>&& tmp)
	:Vector()
{
	Swap(tmp);
}

template<typename Type>
Vector<Type>::~Vector()
{
	Destroy();
}

template<typename Type>
template<typename TReference>
void Vector<Type>::PushBack(TReference && item)
{
	if (!buffLen)
	{
		buffLen = 4;
		AllocData();
	}
	else if (dataLen >= buffLen)
	{
		buffLen *= 1.2f;
		++buffLen;
		EnlargBuff();
	}
	new (&pData[dataLen++]) Type(item);
}

template<typename Type>
void Vector<Type>::PopBack()
{
	assert(dataLen > 0);
	((Type*)&pData[--dataLen])->~Type();
}

template<typename Type>
void Vector<Type>::Erase(unsigned index)
{
	assert(index < dataLen);
	((Type*)&pData[index])->~Type();
	std::memmove((Type*)&pData[index], (Type*)&pData[index + 1], (--dataLen - index) * sizeof(Type));
}

template<typename Type>
void Vector<Type>::Erase(unsigned first, unsigned last)
{
	assert(first <= last && last < dataLen);
	for (unsigned i=first; i<=last; i++)
		((Type*)&pData[i])->~Type();
	dataLen -= ++last - first;
	std::memmove((Type*)&pData[first], (Type*)&pData[last], (dataLen - first) * sizeof(Type));
}

template<typename Type>
void Vector<Type>::Reserve(int n)
{
	if (buffLen < n)
	{
		buffLen = n;
		if (pData)
		{
			EnlargBuff();
		}
		else
		{
			AllocData();
		}
	}
}

template<typename Type>
void Vector<Type>::Resize(int n, Type item = Type())
{
	if (n <= dataLen)	
	{
		for (int i = n; i < dataLen; i++)
			((Type*)&pData[i])->~Type();
	}
	else
	{
		if (n > buffLen)
		{
			buffLen = n;
			EnlargBuff();
		}
		for (int i = dataLen; i < n; i++)
			new (&pData[i]) Type(item);
	}
	dataLen = n;
}

template<typename Type>
void Vector<Type>::Clear()
{
	for (int i=0; i<dataLen; i++)
		((Type*)&pData[i])->~Type();
	dataLen = 0;
}

template<typename Type>
inline Vector<Type>& Vector<Type>::operator=(const Vector<Type>& other)
{
	Destroy();
	buffLen = other.buffLen;
	dataLen = other.dataLen;
	if (other.pData)
	{
		AllocData();
		for (int i = 0; i < dataLen; i++)
			new (&pData[i]) Type(other.pData[i]);
	}
	else
		pData = nullptr;
	return *this;
}

template<typename Type>
inline Vector<Type>& Vector<Type>::operator=(Vector<Type>&& tmp)
{
	Swap(tmp);
	return *this;
}

template<typename Type>
inline void Vector<Type>::Swap(Vector<Type>& other)
{
	std::swap(buffLen, other.buffLen);
	std::swap(dataLen, other.dataLen);
	std::swap(pData, other.pData);
}

template<typename Type>
inline void Vector<Type>::AllocData()
{
	pData = (Type*)std::malloc(buffLen * sizeof(Type));
	assert(pData);
}

template<typename Type>
inline void Vector<Type>::Destroy()
{
	if (pData)
	{
		for (int i = 0; i < dataLen; i++)
			((Type*)&pData[i])->~Type();
		std::free(pData);
	}
}

template<typename Type>
inline void Vector<Type>::EnlargBuff()
{
	Type* newData = (Type*)std::malloc(buffLen * sizeof(Type));
	assert(newData);
	std::memcpy(newData, pData, dataLen * sizeof(Type));
	std::free(pData);
	pData = newData;
}
