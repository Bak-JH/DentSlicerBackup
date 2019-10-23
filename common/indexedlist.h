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
#define EXPOSE_ITR_INDEX
template<class T, class A, class ItrFac>
class DeleteGuard;

namespace IndexedListItr
{
	template <class T, class A >
	class iterator;
	template <class T, class A >
	class const_iterator;
	template <class T, class A >
	class DefaultIteratorFactory;


}

template <class T, class A = std::allocator<T>, class ItrFac = IndexedListItr::DefaultIteratorFactory<T, A>>
class IndexedList {

public:
	typedef A allocator_type;
	typedef typename std::allocator_traits<A> A_trait;
	typedef typename A_trait::value_type value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef typename A_trait::difference_type difference_type;
	typedef typename A_trait::size_type size_type;
	typedef typename std::deque<T, A> container_type;
	typedef typename std::function<void(size_t, size_t)> indexChangedCallback;

	//iterators
	typedef typename ItrFac::iterator iterator;
	typedef typename ItrFac::const_iterator const_iterator;

	typedef std::reverse_iterator<iterator> reverse_iterator; //optional
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator; //optional

	IndexedList(ItrFac itrFac = ItrFac()): _itrFactory(itrFac)
	{
	}
	IndexedList(IndexedList&& other) :
		_container(std::move(other._container)), _indexChangedCallbacks(std::move(other._indexChangedCallbacks)),
		_indexChangedCallbackToken(other._indexChangedCallbackToken), _itrFactory(std::move(other._itrFactory))
	{
	}
	//notice how we do not copy any other members.
	IndexedList(const IndexedList& o) :_container(o._container), _itrFactory(o._itrFactory)
	{
	}
	IndexedList(std::initializer_list<T> iniList, ItrFac itrFac = ItrFac()) : IndexedList(iniList, std::allocator<T>(), itrFac)
	{
	}

	IndexedList(std::initializer_list<T> iniList, const allocator_type& allocator, ItrFac itrFac = ItrFac()) :_container(iniList, allocator), _itrFactory(itrFac)
	{
	}


	virtual ~IndexedList()
	{}
	void setItrFactory(const ItrFac& itrFac)
	{
		_itrFactory = itrFac;
	}
	IndexedList& operator=(IndexedList&& o)
	{
		_container = std::move(o._container);
		_indexChangedCallbacks = std::move(o._indexChangedCallbacks);
		_indexChangedCallbackToken = o._indexChangedCallbackToken;
		return *this;
	}

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
	typename ItrFac::iterator begin();
	typename ItrFac::const_iterator begin() const;
	typename ItrFac::const_iterator cbegin() const;
	typename ItrFac::iterator end();
	typename ItrFac::const_iterator end() const;
	typename ItrFac::const_iterator cend() const;
	typename ItrFac::iterator toNormItr(const const_iterator& itr);
	typename ItrFac::const_iterator toConstItr(const iterator& itr)const;

	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
	const_reverse_iterator crbegin() const;
	reverse_iterator rend();
	const_reverse_iterator rend() const;
	const_reverse_iterator crend() const;



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
	iterator swapAndErase(const_iterator pos);
	//doesn't swap if the element is at the front or back
	iterator swapAndErase(const_iterator start,
		const_iterator end);
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
	void clearIndexChangedCallbacks()
	{
		_indexChangedCallbacks.clear();
		_indexChangedCallbackToken = 0;
	}
	size_t addIndexChangedCallback(indexChangedCallback callback)
	{
		_indexChangedCallbacks[_indexChangedCallbackToken] = callback;
		++_indexChangedCallbackToken;
		return _indexChangedCallbackToken - 1;
	}

	DeleteGuard<T, A, ItrFac> getDeleteGuard()
	{
		return DeleteGuard<T, A, ItrFac>(this);
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
	void modifiedElement(const iterator* itr);
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
	difference_type getItrIndex(const iterator* itr)const;
	ItrFac _itrFactory;
	friend class ItrFac::iterator;
};
template <class T, class A = std::allocator<T> >
void swap(IndexedList<T, A> & a, IndexedList<T, A> & b)
{
	a.swap(b);
}


template<class T, class A, class ItrFac>
class DeleteGuard
{
public:
	DeleteGuard(IndexedList<T, A, ItrFac>* listPtr) : _listPtr(listPtr)
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
	template<class IteratorType>
	void deleteLater(const IteratorType& itr)
	{
#ifdef EXPOSE_ITR_INDEX
		_indices.insert(itr.index());
#else
		_indices.insert(itr - _listPtr->cbegin());
#endif
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
	IndexedList<T, A, ItrFac>* _listPtr = nullptr;
	//have to be decreasing order so that elements about to be deleted do not get swapped
	std::set<size_t, std::greater<size_t>> _indices;
};

//default iterator
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

		iterator() :_owner(nullptr)
		{}
		iterator(size_t index, IndexedList<T, A, DefaultIteratorFactory<T,A>>* owner) : _index(index), _owner(owner)
		{}
		iterator(const iterator& o) : _index(o._index), _owner(o._owner)
		{
		}
		~iterator()
		{}
#ifdef EXPOSE_ITR_INDEX
		size_t index()const
		{
			return _index;
		}
#endif

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
		bool initialized()const
		{
			return _owner != nullptr;
		}
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
		IndexedList<T, A, DefaultIteratorFactory<T, A>>* _owner;
		friend class const_iterator<T, A>;
		friend struct std::hash<IndexedListItr::iterator<T, A>>;

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

		const_iterator() :_owner(nullptr)
		{}
		const_iterator(size_t index, const IndexedList<T, A, DefaultIteratorFactory<T, A>>* containerPtr) : _index(index), _owner(containerPtr)
		{}
		const_iterator(const const_iterator& o) : _index(o._index), _owner(o._owner)
		{}
		const_iterator(const iterator<T, A>& o) : _index(o._index), _owner(o._owner)
		{}
		~const_iterator()
		{}

#ifdef EXPOSE_ITR_INDEX
		size_t index()const
		{
			return _index;
		}
#endif


		const_iterator& operator=(const const_iterator& o)
		{
			_index = o._index;
			_owner = o._owner;
			return *this;
		}
		bool initialized()const
		{
			return _owner != nullptr;
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
		const IndexedList<T, A, DefaultIteratorFactory<T, A>>* _owner;
		friend struct std::hash<IndexedListItr::const_iterator<T, A>>;
	};
	template <class T, class A = std::allocator<T>>
	class DefaultIteratorFactory
	{
	public:
		typedef IndexedListItr::iterator<T, A> iterator;
		typedef  IndexedListItr::const_iterator<T, A> const_iterator;
		DefaultIteratorFactory::iterator buildIterator(size_t index, IndexedList<T, A, DefaultIteratorFactory>* containerPtr)const
		{
			return iterator(index, containerPtr);
		}
		DefaultIteratorFactory::const_iterator buildConstIterator(size_t index, const IndexedList<T, A, DefaultIteratorFactory>* containerPtr)const
		{
			return const_iterator(index, containerPtr);
		}
	};

}



//IndexedList iterator methods
template <class T, class A, class ItrFac>
typename ItrFac::iterator IndexedList<T, A, ItrFac>::begin()
{
	return _itrFactory.buildIterator(0, this);
}
template <class T, class A, class ItrFac>
typename ItrFac::const_iterator IndexedList<T, A, ItrFac>::begin() const
{
	return _itrFactory.buildConstIterator(0, this);
}
template <class T, class A, class ItrFac>
typename ItrFac::const_iterator IndexedList<T, A, ItrFac>::cbegin() const
{
	//auto jj = const_iterator(_container.begin());
	return _itrFactory.buildConstIterator(0, this);
}
template <class T, class A, class ItrFac>
typename ItrFac::iterator IndexedList<T, A, ItrFac>::end()
{
	return _itrFactory.buildIterator(_container.size(), this);
}
template <class T, class A, class ItrFac>
typename ItrFac::const_iterator IndexedList<T, A, ItrFac>::end() const
{
	return _itrFactory.buildConstIterator(_container.size(), this);
}
template <class T, class A, class ItrFac>
typename ItrFac::const_iterator IndexedList<T, A, ItrFac>::cend() const
{
	return _itrFactory.buildConstIterator(_container.size(), this);
}

template <class T, class A, class ItrFac>
typename IndexedList<T, A, ItrFac>::reverse_iterator IndexedList<T, A, ItrFac>::rbegin()
{
	IndexedList<T, A, ItrFac>::reverse_iterator(_container.size(), this);
}
template <class T, class A, class ItrFac>
typename IndexedList<T, A, ItrFac>::const_reverse_iterator IndexedList<T, A, ItrFac>::rbegin() const
{
	return IndexedList<T, A, ItrFac>::const_reverse_iterator(_container.size(), this);
}
template <class T, class A, class ItrFac>
typename IndexedList<T, A, ItrFac>::const_reverse_iterator IndexedList<T, A, ItrFac>::crbegin() const
{
	return IndexedList<T, A, ItrFac>::const_reverse_iterator(_container.size(), this);
}
template <class T, class A, class ItrFac>
typename IndexedList<T, A, ItrFac>::reverse_iterator IndexedList<T, A, ItrFac>::rend()
{
	return IndexedList<T, A, ItrFac>::reverse_iterator(0, this);
}
template <class T, class A, class ItrFac>
typename IndexedList<T, A, ItrFac>::const_reverse_iterator IndexedList<T, A, ItrFac>::rend() const
{
	return IndexedList<T, A, ItrFac>::const_reverse_iterator(0, this);
}
template <class T, class A, class ItrFac>
typename IndexedList<T, A, ItrFac>::const_reverse_iterator IndexedList<T, A, ItrFac>::crend() const
{
	return IndexedList<T, A, ItrFac>::const_reverse_iterator(0, this);
}

template <class T, class A, class ItrFac>
typename ItrFac::iterator IndexedList<T, A, ItrFac>::toNormItr(const typename ItrFac::const_iterator& itr)
{
#ifdef EXPOSE_ITR_INDEX
	auto modItr = begin() + itr.index();
	return modItr;
#else
	size_t idx = itr - cbegin();
	auto modItr = begin() + idx;
	return modItr;

#endif
}
template <class T, class A, class ItrFac>
typename ItrFac::const_iterator IndexedList<T, A, ItrFac>::toConstItr(const typename ItrFac::iterator& itr)const
{
#ifdef EXPOSE_ITR_INDEX
	auto modItr = cbegin() + itr.index();
	return modItr;
#else
	size_t idx = itr - begin();
	auto modItr = cbegin() + idx;
	return modItr;

#endif
}


template <class T, class A, class ItrFac>
typename IndexedList<T, A, ItrFac>::difference_type IndexedList<T, A, ItrFac>::getItrIndex(const typename ItrFac::iterator* itr)const
{
	return ItrFac::const_iterator(*itr) - cbegin();
}
template <class T, class A, class ItrFac>
void IndexedList<T, A, ItrFac>::modifiedElement(const typename ItrFac::iterator* itr)
{
	modifiedElement(getItrIndex(itr));
}
template <class T, class A, class ItrFac>
typename ItrFac::iterator IndexedList<T, A, ItrFac>::swapAndErase(typename ItrFac::const_iterator pos)
{
	auto nextItr = pos;
	++nextItr;
	removedElement(pos - cbegin());
	return swapAndErase(pos, nextItr);

}

template <class T, class A, class ItrFac>
//doesn't swap if the element is at the front or back
typename ItrFac::iterator IndexedList<T, A, ItrFac>::swapAndErase(typename ItrFac::const_iterator start,
	typename ItrFac::const_iterator end)
{
	size_t startedIndex = start - cbegin();
	size_t count = end - start;
	auto currItr = begin() + startedIndex;
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


namespace std
{
	template<class T, class A>
	struct hash<IndexedListItr::iterator<T, A>>
	{
		//2D only!
		std::size_t operator()(const IndexedListItr::iterator<T, A>& itr)const
		{
			return itr._index;
		}
	};

	template<class T, class A>
	struct hash<IndexedListItr::const_iterator<T, A>>
	{
		//2D only!
		std::size_t operator()(const IndexedListItr::const_iterator<T, A>& itr)const
		{
			return itr._index;
		}
	};
}

