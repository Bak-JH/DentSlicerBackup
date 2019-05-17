#pragma once
#include <memory>
#include <deque>
#include <utility>
#include <algorithm>
#include <functional>
#include <map>
#if  defined(QT_DEBUG) || defined(_DEBUG)
#include <stdexcept>
//enforce strict correctness
#define _INDEXED_LIST_STRICT
#endif

template <class T, class A = std::allocator<T>>
class IndexedList {

public:
	typedef A allocator_type;
	typedef typename std::allocator_traits<A> A_trait;
	typedef typename A_trait::value_type value_type;
	typedef typename T& reference;
	typedef typename const T& const_reference;
	typedef typename A_trait::difference_type difference_type;
	typedef typename A_trait::size_type size_type;
	typedef typename std::deque<T, A> container_type;
	typedef typename container_type::iterator container_iterator_type;
	typedef typename container_type::const_iterator container_const_iterator_type;
	typedef typename std::function<void(size_t, size_t)> indexChangedCallback;
	class iterator {
	public:
		typedef typename A_trait::difference_type difference_type;
		typedef typename A_trait::value_type value_type;
		typedef typename T& reference;
		typedef typename A_trait::pointer pointer;
		typedef std::random_access_iterator_tag iterator_category; //or another tag

		iterator()
		{}
		iterator(container_iterator_type containerItr, IndexedList* owner) : _containerItr(containerItr), _owner(owner)
		{}
		iterator(const iterator& o) : _containerItr(o._containerItr), _owner(o._owner)
		{
		}
		~iterator()
		{}

		iterator& operator=(const iterator& o)
		{
			_containerItr = o._containerItr;
		}
		bool operator==(const iterator& o) const
		{
			return _containerItr == o._containerItr;
		}
		bool operator!=(const iterator& o) const
		{
			return _containerItr != o._containerItr;
		}

		//random access iterator
		bool operator< (const iterator& o) const
		{
			return _containerItr < o._containerItr;
		}
		bool operator> (const iterator& o) const
		{
			return _containerItr > o._containerItr;
		}
		bool operator<=(const iterator& o) const
		{
			return _containerItr <= o._containerItr;
		}
		bool operator>=(const iterator& o) const
		{
			return _containerItr >= o._containerItr;
		} //optional

		//pre
		iterator& operator++()
		{
			++_containerItr;
			return *this;
		}
		iterator& operator--()
		{
			--_containerItr;
			return *this;
		}
		//post
		iterator operator++(int)
		{
			auto tmp = *this;
			++_containerItr;
			return tmp;
		}
		iterator operator--(int)
		{
			auto tmp = *this;
			--_containerItr;
			return tmp;

		}

		iterator& operator+=(size_type offset)
		{
			_containerItr += offset;
			return *this;
		}
		iterator& operator-=(size_type offset)
		{
			_containerItr -= offset;

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
		difference_type operator-(const iterator& itr)
		{
			return _containerItr - itr._containerItr;
		}

		reference operator*() const
		{
			_owner->modifiedElement(this);
			return *_containerItr;
		}
		pointer operator->() const
		{
			_owner->modifiedElement(this);
			return &(*_containerItr);
		}
		reference operator[](size_type offset) const
		{
			auto offsetItr = *this + offset;
			_owner->modifiedElement(&offsetItr);
			return *(_containerItr + offset);
		}
	private:
		container_iterator_type _containerItr;
		IndexedList* _owner;

		friend class const_iterator;
	};
	class const_iterator {
	public:
		typedef typename A_trait::difference_type difference_type;
		typedef typename A_trait::value_type value_type;
		typedef typename const T& reference;
		typedef typename const A_trait::const_pointer pointer;

		typedef std::random_access_iterator_tag iterator_category; //or another tag

		const_iterator()
		{}
		const_iterator(container_const_iterator_type containerItr) : _containerItr(containerItr)
		{}
		const_iterator(const const_iterator& o) : _containerItr(o._containerItr)
		{}
		const_iterator(const iterator& o) : _containerItr(o._containerItr)
		{}
		~const_iterator()
		{}

		const_iterator& operator=(const const_iterator& o)
		{
			_containerItr = o._containerItr;
			return *this;

		}
		bool operator==(const const_iterator& o) const
		{
			return _containerItr == o._containerItr;
		}
		bool operator!=(const const_iterator& o) const
		{
			return _containerItr != o._containerItr;
		}
		bool operator<(const const_iterator& o) const
		{
			return _containerItr < o._containerItr;
		}
		bool operator>(const const_iterator& o) const
		{
			return _containerItr > o._containerItr;
		}
		bool operator<=(const const_iterator& o) const
		{
			return _containerItr <= o._containerItr;
		}
		bool operator>=(const const_iterator& o) const
		{
			return _containerItr >= o._containerItr;
		} //optional
				//pre
		const_iterator& operator++()
		{
			++_containerItr;
			return *this;
		}
		const_iterator& operator--()
		{
			--_containerItr;
			return *this;
		}
		//post
		const_iterator operator++(int)
		{
			auto tmp = *this;
			++_containerItr;
			return tmp;
		}
		const_iterator operator--(int)
		{
			auto tmp = *this;
			--_containerItr;
			return tmp;
		}
		const_iterator& operator+=(size_type offset)
		{
			_containerItr += offset;
			return *this;
		}
		const_iterator& operator-=(size_type offset)
		{
			_containerItr -= offset;
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
		difference_type operator-(const const_iterator& itr)
		{
			return _containerItr - itr._containerItr;
		}

		reference operator*() const
		{
			return *_containerItr;
		}
		pointer operator->() const
		{
			return &(*_containerItr);

		}
		reference operator[](size_type offset) const
		{
			return *( _containerItr + offset);
		}
	private:
		container_const_iterator_type _containerItr;
	};

	typedef std::reverse_iterator<iterator> reverse_iterator; //optional
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator; //optional

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
	iterator begin()
	{
		return iterator(_container.begin(), this);
	}
	const_iterator begin() const
	{
		return const_iterator(_container.begin());
	}
	const_iterator cbegin() const
	{
		//auto jj = const_iterator(_container.begin());
		return const_iterator(_container.cbegin());

	}
	iterator end()
	{
		return iterator(_container.end(), this);

	}
	const_iterator end() const
	{
		return const_iterator(_container.cend());

	}
	const_iterator cend() const
	{
		return const_iterator(_container.cend());
	}
	reverse_iterator rbegin()
	{
		reverse_iterator(end());
	}
	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(end());

	}
	const_reverse_iterator crbegin() const
	{
		return const_reverse_iterator(end());
	}
	reverse_iterator rend()
	{
		return reverse_iterator(begin());

	}
	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(begin());

	}
	const_reverse_iterator crend() const
	{
		return const_reverse_iterator(begin());

	}
	//iterator conversions
	iterator toNormItr(const_iterator itr)
	{
		size_t idx = itr - cbegin();
		return  begin() + idx;
	}


	template<class ...Args>
	void emplace_front(Args&& ... args)
	{
		_container.emplace_front(std::forward< Args>(args)...);
		addedElement(0);
	}
	template<class ...Args>
	void emplace_back(Args&& ... args)
	{
		addedElement(size());
		_container.emplace_back(std::forward< Args>(args)...);
	}
	void push_front(const T& e)
	{
		_container.push_front(e);
		addedElement(0);
	}
	void push_front(T&& e)
	{
		_container.push_front(std::forward<T>(e));
		addedElement(0);

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
	void pop_front()
	{
		_container.pop_front();
		removedElement(0);
	}
	void pop_back()
	{
		removedElement(size());
		_container.pop_back();
	}

	//doesn't swap if the element is at the front or back
	iterator swapAndErase(const_iterator pos, bool allowPopFront = false)
	{
		auto nextItr = pos;
		++nextItr;
		removedElement(pos - cbegin());
		return swapAndErase(pos, nextItr);

	}
	//doesn't swap if the element is at the front or back
	iterator swapAndErase(const_iterator start, const_iterator end, bool allowPopFront = false)
	{
		size_t startedIndex = start - cbegin();
		size_t count = end - start;
		iterator currItr = begin() + startedIndex;
		size_t index = 0;
		size_t remaining = count;
		// n amount of deletes, maximum n amounts of swaps
		while(remaining)
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
			else if (allowPopFront && currItr == begin())
			{
				while (remaining)
				{
					removedElement(0);
					_container.pop_front();
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

	size_type size()
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
	void modifiedElement(const iterator* itr)
	{
		modifiedElement(getItrIndex(itr));
	}
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
	inline difference_type getItrIndex(const iterator* itr)const
	{
		return const_iterator(*itr) - cbegin();
	}
};
template <class T, class A = std::allocator<T> >
void swap(IndexedList<T, A> & a, IndexedList<T, A> & b)
{
	a.swap(b);
}