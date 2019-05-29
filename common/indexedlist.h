#pragma once
#include <memory>
#include <deque>
#include <utility>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#if  defined(QT_DEBUG) || defined(_DEBUG)
#include <stdexcept>
//enforce strict correctness
#define _INDEXED_LIST_STRICT
#endif

template<class T, class A>
class DeleteGuard;

namespace IndexedListItr
{
	template <class T, class A >
	class iterator;
	template <class T, class A >
	class const_iterator;



}

template <class T, class A = std::allocator<T>>
class IndexedList {

public:
	typedef A allocator_type;
	typedef typename std::allocator_traits<A> A_trait;
	typedef typename A_trait::value_type value_type;
    typedef T& reference;
    typedef const T& const_reference;
	typedef typename A_trait::difference_type difference_type;
	typedef typename A_trait::size_type size_type;
	typedef typename std::deque<T,A> container_type;
	//typedef typename boost::container::stable_vector_iterator<T*, false> container_iterator_type;
	//typedef typename boost::container::stable_vector_iterator<T*, true> container_const_iterator_type;
	typedef typename std::function<void(size_t, size_t)> indexChangedCallback;
	typedef std::reverse_iterator<IndexedListItr::iterator<T, A>> reverse_iterator; //optional
	typedef std::reverse_iterator<IndexedListItr::const_iterator<T, A>> const_reverse_iterator; //optional

	IndexedList()
	{

	}
	IndexedList(const IndexedList& o) :_container(o._container)
	{
	}
	IndexedList(std::initializer_list<T> iniList) : IndexedList(iniList, std::allocator<T>())
	{
	}

	IndexedList(std::initializer_list<T> iniList, const allocator_type& allocator) :_container(iniList, allocator)
	{
	}


	virtual ~IndexedList()
	{}

	IndexedList& operator=(const IndexedList& o)
	{
		_container = o._container;
		return *this;
	}

	IndexedList& operator=(std::initializer_list<T> iniList) { // assign initializer_list
		assign(iniList.begin(), iniList.end());
		return *this;
	}
	bool operator==(const IndexedList& o) const
	{
		return _container == o._container;
	}
	bool operator!=(const IndexedList& o) const
	{
		return _container != o._container;

	}
	//simple equal and not equal comparison should be enough
	//bool operator<(const IndexedList&) const; //optional
	//bool operator>(const IndexedList&) const; //optional
	//bool operator<=(const IndexedList&) const; //optional
	//bool operator>=(const IndexedList&) const; //optional

	//iterators
	IndexedListItr::iterator<T, A> begin()					   ;
	IndexedListItr::const_iterator<T, A> begin() const		   ;
	IndexedListItr::const_iterator<T, A> cbegin() const		   ;
	IndexedListItr::iterator<T, A> end()						   ;
	IndexedListItr::const_iterator<T, A> end() const			   ;
	IndexedListItr::const_iterator<T, A> cend() const		   ;
	reverse_iterator rbegin()			   ;
	const_reverse_iterator rbegin() const ;
	const_reverse_iterator crbegin() const;
	reverse_iterator rend()			   ;
	const_reverse_iterator rend() const   ;
	const_reverse_iterator crend() const  ;
    IndexedListItr::iterator<T, A> toNormItr(const IndexedListItr::const_iterator<T,A>& itr);



	template<class ...Args>
	void emplace_back(Args&& ... args)
	{
		addedElement(size());
		_container.emplace_back(std::forward< Args>(args)...);
	}
	void push_back(const T& e)
	{
		addedElement(size());
		_container.push_back(e);
	}
	void push_back(T&& e)
	{
		addedElement(size());
		_container.push_back(std::forward<T>(e));
	}
	void pop_back()
	{
		removedElement(size());
		_container.pop_back();
	}
	IndexedListItr::iterator<T, A> swapAndErase(IndexedListItr::const_iterator<T, A> pos);
	//doesn't swap if the element is at the front or back
	IndexedListItr::iterator<T, A> swapAndErase(IndexedListItr::const_iterator<T, A> start,
		IndexedListItr::const_iterator<T, A> end);
	void clear()
	{
		_container.clear();
		removedAll();
	}
	template<class iter>
	void assign(iter first, iter last)
	{
		_container.assign(first, last);
		addedAll();
	}
	void assign(std::initializer_list<T> list)
	{
		_container.assign(std::forward<std::initializer_list<T>>(list));
		addedAll();
	}
	void assign(size_type count, const T& e)
	{
		_container.assign(count, { 0, e });
		addedAll();

	}

	void swap(IndexedList& o)
	{
		_container.swap(o._container);
		addedAll();
	}

	//accessors

	reference front()
	{
		modifiedElement((size_t)0);
		return _container.front();
	}
	const_reference front() const
	{
		const auto* constCont = &_container;
		return constCont->front();

	}
	reference back()
	{
		modifiedElement(size() - 1);
		return _container.back();
	}
	const_reference back() const
	{
		return _container.back();
	}
	reference operator[](size_type index)
	{
		modifiedElement(index);
		return _container[index];
	}
	const_reference operator[](size_type index) const
	{
		return _container[index];

	}
	reference at(size_type index)
	{
		modifiedElement(index);
		return _container.at(index);
	}
	const_reference at(size_type index) const
	{
		return _container.at(index);

	}

	size_type size() const 
	{
		return _container.size();
	}
	size_type max_size() const
	{
		return _container.max_size();
	}
	bool empty() const
	{
		return _container.empty();
	}

	A get_allocator() const
	{
		return _container.get_allocator();

	}
	size_t addIndexChangedCallback(indexChangedCallback callback)
	{
		_indexChangedCallbacks[_indexChangedCallbackToken] = callback;
		++_indexChangedCallbackToken;
		return _indexChangedCallbackToken - 1;
	}

	DeleteGuard<T, A> getDeleteGuard()
	{
		return DeleteGuard<T, A>(this);
	}

protected:
	virtual void addedElement(size_t index)
	{
	}
	virtual void addedAll()
	{
	}
	virtual void removedElement(size_t index)
	{
	}
	virtual void removedAll()
	{
	}
	virtual void modifiedElement(size_t index)
	{
	}
	void modifiedElement(const IndexedListItr::iterator<T, A>* itr);
private:
	container_type _container;
	std::map<size_t, indexChangedCallback> _indexChangedCallbacks;
	size_t _indexChangedCallbackToken = 0;

	void indexChanged(size_t oldIndex, size_t newIndex)
	{
		for (auto pair : _indexChangedCallbacks)
		{
			auto& callback = pair.second;
			callback(oldIndex, newIndex);
		}
	}
	typename IndexedList<T, A>::difference_type getItrIndex(const IndexedListItr::iterator<T, A>* itr)const;
	friend class IndexedListItr::iterator<T,A>;
};
template <class T, class A = std::allocator<T> >
void swap(IndexedList<T, A> & a, IndexedList<T, A> & b)
{
	a.swap(b);
}


template<class T, class A>
class DeleteGuard
{
public:
	DeleteGuard(IndexedList<T, A>* listPtr) : _listPtr(listPtr)
	{
	}
	~DeleteGuard()
	{
		flush();
	}
	DeleteGuard(const DeleteGuard&) = delete;
	DeleteGuard& operator=(const DeleteGuard&) = delete;
	DeleteGuard(DeleteGuard&& o) noexcept :
		_listPtr(o._listPtr),       // explicit move of a member of class type
		_indices(std::move(o._indices))
	{
		o._listPtr = nullptr;
	}
	DeleteGuard& operator=(DeleteGuard&& o)
	{
		flush();
		_listPtr = o._listPtr;
		o._listPtr = nullptr;
        _indices = std::move(o._indices);
	}
	void deleteLater(typename IndexedListItr::const_iterator<T, A> itr)
	{
		size_t idx = itr - _listPtr->cbegin();
		_indices.insert(idx);
	}
	void flush()
	{
		if (_listPtr)
		{
			for (auto idx : _indices)
			{
				_listPtr->swapAndErase(_listPtr->cbegin() + idx);
			}
			_indices.clear();
		}
	}
private:
	IndexedList<T, A>* _listPtr = nullptr;
	//have to be decreasing order so that elements about to be deleted do not get swapped
	std::set<size_t, std::greater<size_t>> _indices;
};

namespace IndexedListItr
{

	template <class T, class A = std::allocator<T>>
	class iterator {
	public:
		typedef A allocator_type;
		typedef typename std::allocator_traits<A> A_trait;
		typedef typename A_trait::size_type size_type;
		//traits
		typedef typename A_trait::difference_type difference_type;
		typedef typename A_trait::value_type value_type;
        typedef value_type& reference;
		typedef typename A_trait::pointer pointer;
		typedef std::random_access_iterator_tag iterator_category; //or another tag

		iterator()
		{}
		iterator(size_t index, IndexedList<T,A>* owner) : _index(index), _owner(owner)
		{}
		iterator(const iterator& o) : _index(o._index), _owner(o._owner)
		{
		}
		~iterator()
		{}

		iterator& operator=(const iterator& o)
		{
			_index = o._index;
			_owner = o._owner;
			return *this;
		}
		bool operator==(const iterator& o) const
		{
			return _index == o._index;
		}
		bool operator!=(const iterator& o) const
		{
			return !operator==(o);
		}

		//random access iterator
		bool operator< (const iterator& o) const
		{
			return _index < o._index;
		}
		bool operator> (const iterator& o) const
		{
			return _index > o._index;
		}
		bool operator<=(const iterator& o) const
		{
			return _index <= o._index;
		}
		bool operator>=(const iterator& o) const
		{
			return _index >= o._index;
		} //optional

		//pre
		iterator& operator++()
		{
			++_index;
			return *this;
		}
		iterator& operator--()
		{
			--_index;
			return *this;
		}
		//post
		iterator operator++(int)
		{
			auto tmp = *this;
			++_index;
			return tmp;
		}
		iterator operator--(int)
		{
			auto tmp = *this;
			--_index;
			return tmp;

		}
		iterator& operator+=(size_type offset)
		{
			_index += offset;
			return *this;
		}
		iterator& operator-=(size_type offset)
		{
			_index -= offset;

			return *this;
		}
		iterator operator+(size_type offset) const
		{
			auto tmp = *this;
			return tmp += offset;
		}
        friend iterator operator+(size_type offset, const iterator& itr)
		{
			return itr += offset;

		}
		iterator operator-(size_type offset) const
		{
			auto tmp = *this;
			return tmp -= offset;
		}
        difference_type operator-(const iterator& itr)const
		{
			return _index - itr._index;
		}

		reference operator*() const
		{
			_owner->modifiedElement(this);
			return _owner->operator[](_index);
		}
		pointer operator->() const
		{
			_owner->modifiedElement(this);
			return &(_owner->operator[](_index));
		}
		reference operator[](size_type offset) const
		{
			auto offsetItr = *this + offset;
			_owner->modifiedElement(&offsetItr);
			return _owner->operator[](_index + offset);
		}
	private:
		size_t _index;
		IndexedList<T,A>* _owner;

		friend class const_iterator<T,A>;
	};

	template <class T, class A = std::allocator<T>>
	class const_iterator {
	public:
		typedef A allocator_type;
		typedef typename std::allocator_traits<A> A_trait;
		typedef typename A_trait::size_type size_type;
		//traits
		typedef typename A_trait::difference_type difference_type;
		typedef typename A_trait::value_type value_type;
        typedef const value_type& reference;
        typedef typename A_trait::const_pointer pointer;
		typedef std::random_access_iterator_tag iterator_category; //or another tag

		const_iterator()
		{}
		const_iterator(size_t index, const IndexedList<T, A>* containerPtr) : _index(index), _owner(containerPtr)
		{}
		const_iterator(const const_iterator& o) : _index(o._index), _owner(o._owner)
		{}
		const_iterator(const iterator<T,A>& o) : _index(o._index), _owner(o._owner)
		{}
		~const_iterator()
		{}





		const_iterator& operator=(const const_iterator& o)
		{
			_index = o._index;
			_owner = o._owner;
			return *this;
		}
		bool operator==(const const_iterator& o) const
		{
			return _index == o._index;
		}
		bool operator!=(const const_iterator& o) const
		{
			return !operator==(o);
		}

		//random access const_iterator
		bool operator< (const const_iterator& o) const
		{
			return _index < o._index;
		}
		bool operator> (const const_iterator& o) const
		{
			return _index > o._index;
		}
		bool operator<=(const const_iterator& o) const
		{
			return _index <= o._index;
		}
		bool operator>=(const const_iterator& o) const
		{
			return _index >= o._index;
		} //optional

		//pre
		const_iterator& operator++()
		{
			++_index;
			return *this;
		}
		const_iterator& operator--()
		{
			--_index;
			return *this;
		}
		//post
		const_iterator operator++(int)
		{
			auto tmp = *this;
			++_index;
			return tmp;
		}
		const_iterator operator--(int)
		{
			auto tmp = *this;
			--_index;
			return tmp;

		}
		const_iterator& operator+=(size_type offset)
		{
			_index += offset;
			return *this;
		}
		const_iterator& operator-=(size_type offset)
		{
			_index -= offset;

			return *this;
		}
		const_iterator operator+(size_type offset) const
		{
			auto tmp = *this;
			return tmp += offset;
		}
		friend const_iterator operator+(size_type offset, const const_iterator& itr)
		{
			return itr += offset;

		}
		const_iterator operator-(size_type offset) const
		{
			auto tmp = *this;
			return tmp -= offset;
		}
		difference_type operator-(const const_iterator& itr)const
		{
			return _index - itr._index;
		}

		reference operator*() const
		{
			return _owner->operator[](_index);
		}
		pointer operator->() const
		{
			return &(_owner->operator[](_index));
		}
		reference operator[](size_type offset) const
		{
			return _owner->operator[](_index + offset);
		}
	private:
		size_t _index;
		const IndexedList<T, A>* _owner;

	};

}

//IndexedList iterator methods
template <class T, class A>
IndexedListItr::iterator<T, A> IndexedList<T,A>::begin()
{
	return IndexedListItr::iterator<T, A>(0, this);
}
template <class T, class A>
IndexedListItr::const_iterator<T, A> IndexedList<T, A>::begin() const
{
	return IndexedListItr::const_iterator<T, A>(0, this);
}
template <class T, class A>
IndexedListItr::const_iterator<T, A> IndexedList<T, A>::cbegin() const
{
	//auto jj = const_iterator(_container.begin());
	return IndexedListItr::const_iterator<T, A>(0, this);
}
template <class T, class A>
IndexedListItr::iterator<T, A> IndexedList<T, A>::end()
{
	return IndexedListItr::iterator<T, A>(_container.size(), this);
}
template <class T, class A>
IndexedListItr::const_iterator<T, A> IndexedList<T, A>::end() const
{
	return IndexedListItr::const_iterator<T, A>(_container.size(), this);
}
template <class T, class A>
IndexedListItr::const_iterator<T, A> IndexedList<T, A>::cend() const
{
	return IndexedListItr::const_iterator<T, A>(_container.size(), this);
}

template <class T, class A>
typename IndexedList<T,A>::reverse_iterator IndexedList<T, A>::rbegin()
{
	IndexedList<T, A>::reverse_iterator(_container.size(), this);
}
template <class T, class A>
typename IndexedList<T, A>::const_reverse_iterator IndexedList<T, A>::rbegin() const
{
	return IndexedList<T, A>::const_reverse_iterator(_container.size(), this);
}
template <class T, class A>
typename IndexedList<T, A>::const_reverse_iterator IndexedList<T, A>::crbegin() const
{
	return IndexedList<T, A>::const_reverse_iterator(_container.size(), this);
}
template <class T, class A>
typename IndexedList<T, A>::reverse_iterator IndexedList<T, A>::rend()
{
	return IndexedList<T, A>::reverse_iterator(0, this);
}
template <class T, class A>
typename IndexedList<T, A>::const_reverse_iterator IndexedList<T, A>::rend() const
{
	return IndexedList<T, A>::const_reverse_iterator(0, this);
}
template <class T, class A>
typename IndexedList<T, A>::const_reverse_iterator IndexedList<T, A>::crend() const
{
	return IndexedList<T, A>::const_reverse_iterator(0, this);
}

template <class T, class A>
IndexedListItr::iterator<T, A> IndexedList<T, A>::toNormItr(const IndexedListItr::const_iterator<T,A>& itr)
{
    size_t idx = itr - cbegin();
	return  begin() + idx;
}
template <class T, class A>
typename IndexedList<T,A>::difference_type IndexedList<T, A>::getItrIndex(const IndexedListItr::iterator<T, A>* itr)const
{
	return IndexedListItr::const_iterator<T, A>(*itr) - cbegin();
}
template <class T, class A>
void IndexedList<T, A>::modifiedElement(const IndexedListItr::iterator<T, A>* itr)
{
	modifiedElement(getItrIndex(itr));
}
template <class T, class A>
IndexedListItr::iterator<T,A> IndexedList<T, A>::swapAndErase(IndexedListItr::const_iterator<T,A> pos)
{
	auto nextItr = pos;
	++nextItr;
	removedElement(pos - cbegin());
	return swapAndErase(pos, nextItr);

}
template <class T, class A>
//doesn't swap if the element is at the front or back
IndexedListItr::iterator<T, A> IndexedList<T, A>::swapAndErase(IndexedListItr::const_iterator<T, A> start,
	IndexedListItr::const_iterator<T, A> end)
{
	size_t startedIndex = start - cbegin();
	size_t count = end - start;
	IndexedListItr::iterator<T, A> currItr = begin() + startedIndex;
	size_t index = 0;
	size_t remaining = count;
	// n amount of deletes, maximum n amounts of swaps
	while (remaining)
	{
		//don't swap and just pop if the to be erased is at the back or front
		if (currItr == this->end() - remaining)
		{
			while (remaining)
			{
				_container.pop_back();
				removedElement(size());
				--remaining;
			}
		}
		else
		{
			index = currItr - begin();
			std::swap(*(currItr), _container.back());
			//in case index order is important, inform that an index was changed
			indexChanged(_container.size() - 1, index);
			modifiedElement(index);
			_container.pop_back();
			removedElement(size());
			--remaining;
			++currItr;
		}

	}
	//iterator can be invalidated when deleted
	return begin() + startedIndex;
}


