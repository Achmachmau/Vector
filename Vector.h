#pragma once
#include <cassert>

class ResizePolicy	//или без класса, просто глобальные переменные
{
public:
	static short int beginVolume;	//начальный объем выделяемой памяти
	static float constEnlarg;	  //множитель при расширении
};

//Объявить глобально
short int ResizePolicy::beginVolume = 128;
float ResizePolicy::constEnlarg = 1.2f;

/*export*/ template <typename T>	//нет больше export, всю реализацию засовываем в один файл
class Vector
{
private:	
	int capacity;
	int length;
	T* data;
public:
	explicit Vector();
	explicit Vector(int size, const T& value);
	Vector(const Vector<T>& A);
	~Vector();
	void PushBack(const T& item);
	void PopBack()
	{
		//якобы удаляем
		assert(this->length > 0);
		--this->length;
	}
	void Erase(int k);	//удаление k-го элемента
	void Reserve(int n);	//выделение памяти под n элементов
	void Resize(int n, T item = T());	//изменение размера вектора
	void Clear()	//очистка вектора
	{
		this->length = 0;	//значения можно и не занулять
	}
	int Size() { return this->length; }
	int Capacity() { return this->capacity; }
	
	T& operator[](int index)
	{
		assert(index >= 0 && index < this->length);
		return data[index];
	}
};

template<typename T> Vector<T>::Vector()
{
	//Выделяем "про запас" несколько элементов общим объемом ResizePolicy::beginVolume
	this->capacity = ResizePolicy::beginVolume / sizeof(T);
	this->length = 0;
	this->data = new T[this->capacity];
}

template<typename T> Vector<T>::Vector(int size, const T& value = T())
{
	//Выделяем память для n элементов (хозяин-барин, запас не делаем)
	this->capacity = size;
	this->length = size;
	this->data = new T[this->capacity];
	for (int i = 0; i < size; i++)
		this->data[i] = value;
}

template<typename T> Vector<T>::Vector(const Vector<T>& A)
{
	this->capacity = A.capacity;
	this->length = A.length;
	this->data = new T[this->capacity];
	for (int i = 0; i < this->length; i++)
		this->data[i] = A.data[i];
}

template<typename T> Vector<T>::~Vector()
{
	assert(this->capacity > 0);
	delete[] this->data;
}

template<typename T> void Vector<T>::PushBack(const T& item)
{
	if (this->length >= this->capacity)
	{
		//расширяем
		this->capacity *= ResizePolicy::constEnlarg;
		T* newData = new T[this->capacity];
		int moduloLength = this->length % 4;
		for (int i = 0; i < this->length - moduloLength; i+=4)	//а развёртки то хороши (даже такие кривые)
		{
			newData[i] = this->data[i];
			newData[i + 1] = this->data[i + 1];
			newData[i + 2] = this->data[i + 2];
			newData[i + 3] = this->data[i + 3];
		}
		while (moduloLength)
		{
			newData[this->length - moduloLength] = this->data[this->length - moduloLength];
			--moduloLength;
		}
		T* deleteData = this->data;
		this->data = newData;
		delete[] deleteData;
	}
	this->data[this->length++] = item;
}

template<typename T> void Vector<T>::Erase(int k)
{
//ужасно медленно даже с развёрткой
	int modulo = (this->length-k) % 4;
	for (int i = k; i < this->length - modulo; i+=4)
	{
		this->data[i] = this->data[i + 1];
		this->data[i + 1] = this->data[i + 2];
		this->data[i + 2] = this->data[i + 3];
		this->data[i + 3] = this->data[i + 4];
	}
	while (modulo>1)
	{
		this->data[this->length - modulo] = this->data[this->length - modulo + 1];
		--modulo;
	}
	--this->length;
}

template<typename T> void Vector<T>::Reserve(int n)
{
	this->capacity = n;
	if (n < this->length)
		this->length = n;
	T* newData = new T[this->capacity];
	for (int i = 0; i < this->length; i++)
	{
		newData[i] = this->data[i];
	}
	T* deleteData = this->data;
	this->data = newData;
	delete[] deleteData;
}

template<typename T> void Vector<T>::Resize(int n, T item = T())
{
	if (n > this->capacity)
	{
		//Расширяем до n (без запаса)
		this->capacity = n;
		//копипаст из PushBack()
		T* newData = new T[this->capacity];
		int moduloLength = this->length % 4;
		for (int i = 0; i < this->length - moduloLength; i += 4)
		{
			newData[i] = this->data[i];
			newData[i + 1] = this->data[i + 1];
			newData[i + 2] = this->data[i + 2];
			newData[i + 3] = this->data[i + 3];
		}
		while (moduloLength)
		{
			newData[this->length - moduloLength] = this->data[this->length - moduloLength];
			--moduloLength;
		}
		T* deleteData = this->data;
		this->data = newData;
		delete[] deleteData;
	}
	for (int i = this->length; i < n; i++)
	{
		this->data[i] = item;
	}
	this->length = n;
}
