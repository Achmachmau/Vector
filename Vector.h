#pragma once
#include <cassert>

template <typename Type>
class Vector
{
private:
	unsigned buffLen;
	unsigned dataLen;
	Type* pData;
public:
	Vector() : buffLen(0), dataLen(0), pData(nullptr) {};
	explicit Vector(unsigned n)
		:buffLen(n), dataLen(0), pData(nullptr)
	{
		if (n) {
			Allocate(n); }
	}
	explicit Vector(unsigned n, const Type& item)
		:buffLen(n), dataLen(n), pData(nullptr)
	{
		if (n)
		{
			Allocate(n);
			for (unsigned i = 0; i < n; ++i)
				new (&pData[i]) Type(item);
		}
	}
	Vector(const Vector<Type>& other) : Vector()
	{
		if (other.dataLen)
		{
			Allocate(other.dataLen);
			dataLen = other.dataLen;
			for (unsigned i = 0; i < dataLen; ++i)
				new (&pData[i]) Type(other.pData[i]);
		}
	}
	Vector(Vector<Type>&& tmp) : Vector() { Swap(tmp); }
	~Vector() { Destroy(); }

	void PushBack(const Type& item)
	{
		Allocate(dataLen + 1);
		new (&pData[dataLen++]) Type(item);
	}
	template<typename ...TArg>
	void PushBack(TArg&&... arg)
	{
		Allocate(dataLen + 1);
		new (&pData[dataLen++]) Type(std::forward<TArg>(arg)...);
	}
	void PopBack()
	{
		assert(dataLen > 0);
		((Type*)&pData[--dataLen])->~Type();
	}
	void PushFront(const Type& item)
	{
		Insert(0, item);
	}
	template<typename ...TArg>
	void PushFront(TArg&&... arg)
	{
		Insert(0, std::forward<TArg>(arg)...);
	}
	void PopFront()
	{
		Erase(0);
	}
	void Erase(unsigned index)
	{
		assert(index < dataLen);
		((Type*)&pData[index])->~Type();
		std::memmove((Type*)&pData[index], (Type*)&pData[index + 1], (--dataLen - index) * sizeof(Type));
	}
	void Erase(unsigned first, unsigned last)
	{
		assert(first <= last && last < dataLen);
		for (unsigned i = first; i <= last; ++i)
			((Type*)&pData[i])->~Type();
		dataLen -= ++last - first;
		std::memmove((Type*)&pData[first], (Type*)&pData[last], (dataLen - first) * sizeof(Type));
	}
	void Insert(unsigned index, const Type& item)
	{
		assert(index < dataLen);
		Allocate(dataLen + 1);
		std::memmove((Type*)&pData[index+1], (Type*)&pData[index], (dataLen++ - index) * sizeof(Type));
		new (&pData[index]) Type(item);
	}
	template<typename ...TArg>
	void Insert(unsigned index, TArg&&... arg)
	{
		assert(index < dataLen);
		Allocate(dataLen + 1);
		std::memmove((Type*)&pData[index + 1], (Type*)&pData[index], (dataLen++ - index) * sizeof(Type));
		new (&pData[index]) Type(std::forward<TArg>(arg)...);
	}
	void Reserve(unsigned n)
	{
		Allocate(n);
	}
	void Resize(unsigned n, const Type& item = Type())
	{
		if (n <= dataLen)
		{
			for (unsigned i = n; i < dataLen; ++i)
				((Type*)&pData[i])->~Type();
		}
		else
		{
			Allocate(n);
			for (unsigned i = dataLen; i < n; ++i)
				new (&pData[i]) Type(item);
		}
		dataLen = n;
	}
	void Reverse(unsigned first = 0, unsigned last = 0)
	{
		if (!first && !last && dataLen)
			last = dataLen - 1;
		assert(first <= last && last < dataLen);
		Type tmp;
		for (unsigned i = 0; i < (last - first + 1) / 2; ++i)
		{
			tmp = std::move(pData[first + i]);
			pData[first + i] = std::move(pData[last - i]);
			pData[last - i] = std::move(tmp);
		}
	}
	void Clear(bool bClearBuff = false) { Destroy(bClearBuff); }
	void ShrinkToFit()
	{
		if (buffLen > dataLen)
		{
			buffLen = dataLen;
			Allocate(buffLen, true);
		}
	}
	unsigned Size() { return dataLen; }
	unsigned Capacity() { return buffLen; }
	bool Empty() { return !dataLen; }

	Vector<Type>& operator=(const Vector<Type>& other)
	{
		Destroy(false);		//destroy objects in data, buffer not cleared
		if (other.dataLen)
		{
			Allocate(other.dataLen);
			dataLen = other.dataLen;
			for (unsigned i = 0; i < dataLen; ++i)
				new (&pData[i]) Type(other.pData[i]);
		}
		return *this;
	}
	Vector<Type>& operator=(Vector<Type>&& tmp)
	{
		Swap(tmp);
		return *this;
	}
	Type& operator[](unsigned index)
	{
		assert(index < dataLen);
		return pData[index];
	}
	const Type& operator[](unsigned index) const
	{
		assert(index < dataLen);
		return pData[index];
	}

	void InsertSort()
	{
		Type x;
		unsigned j;
		for (unsigned i = 1; i < dataLen; ++i)
		{
			x = pData[i];
			j = i - 1;
			while (j >= 0 && pData[j] > x)
			{
				pData[j + 1] = std::move(pData[j]);
				pData[j] = std::move(x);
				--j;
			}
		}
	}
	void SelectSort()
	{
		Type min;
		unsigned indM;
		for (unsigned i = 0; i < dataLen - 1; ++i)
		{
			min = pData[i];
			indM = i;
			for (unsigned j = i + 1; j < dataLen; j++)
			{
				if (pData[j] < min)
				{
					min = pData[j];
					indM = j;
				}
			}
			if (indM != i)
			{
				pData[indM] = std::move(pData[i]);
				pData[i] = std::move(min);
			}
		}
	}
	void BubbleSort()
	{
		bool bSwap = true;
		if (dataLen <= 1) {
			return; }
		unsigned endInd = dataLen - 1;
		while (bSwap)
		{
			bSwap = false;
			for (unsigned i = 0; i < endInd; ++i)
			{
				if (pData[i] > pData[i + 1])
				{
					std::swap(pData[i], pData[i + 1]);
					bSwap = true;
				}
			}
			--endInd;
		}
	}
	void QuickSort(Type* bgnItm = nullptr, Type* endItm = nullptr)
	{
		if (!bgnItm && !endItm)
		{
			bgnItm = &pData[0];
			endItm = &pData[dataLen - 1];
		}
		if (endItm - bgnItm < 1) {
			return; }

		Type pivot = *(bgnItm + (endItm - bgnItm) / 2);
		Type* left = bgnItm, *right = endItm;
		while (left <= right) {
			while (*left < pivot) {
				++left; }
			while (*right > pivot) {
				--right; }
			if (left <= right)
			{
				std::swap(*left, *right);
				++left;
				--right;
			}
		}
		if (bgnItm < right)
			QuickSort(bgnItm, right + 1);
		if (left < endItm)
			QuickSort(left, endItm);
	}

	Type* begin() { return pData; }
	Type* end() { return &pData[dataLen]; }

private:
	void Swap(Vector<Type>& other)
	{
		std::swap(buffLen, other.buffLen);
		std::swap(dataLen, other.dataLen);
		std::swap(pData, other.pData);
	}
	void Allocate(unsigned n, bool bShrink = false)
	{
		if (!pData && !bShrink)
		{
			buffLen = 128 / sizeof(Type);
			if (buffLen < n) {
				buffLen = n; }
			pData = (Type*)std::malloc(buffLen * sizeof(Type));
			assert(pData);
		}
		else if (n > buffLen || bShrink)
		{
			if (!bShrink)
			{
				buffLen = buffLen * 6 / 5;
				if (n > buffLen) {
					buffLen = n;
				}
			}
			Type* newData = (Type*)std::malloc(buffLen * sizeof(Type));
			assert(newData);
			std::memcpy(newData, pData, dataLen * sizeof(Type));
			std::free(pData);
			pData = newData;
		}
	}
	void Destroy(bool bFreeData = true)
	{
		if (pData)
		{
			for (unsigned i = 0; i < dataLen; ++i) {
				((Type*)&pData[i])->~Type(); }
			dataLen = 0;
			if (bFreeData)
			{
				std::free(pData);
				buffLen = 0;
				pData = nullptr;
			}
		}
	}
};
