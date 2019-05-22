#pragma once


//don't use traits here as that may create circular dependency
template <class iterator_type, class value_type>
class IteratorWrapper
{
public:
	IteratorWrapper() {
	}
	IteratorWrapper(iterator_type* ptr) : _itrPtr(ptr)
	{}
	IteratorWrapper(const IteratorWrapper& o)
	{
		if (o._itrPtr != nullptr)
		{
			_itrPtr = std::unique_ptr<iterator_type>(new iterator_type(o.getItr()));
		}
	}
	IteratorWrapper& operator=(const IteratorWrapper& o)
	{
		if (o._itrPtr != nullptr)
		{
			_itrPtr = std::unique_ptr<iterator_type>(new iterator_type(o.getItr()));
		}		return *this;
	}

	value_type& operator*() const
	{
		return **_itrPtr;
	}
	value_type* operator->() const
	{
		return &(**_itrPtr);

	}
	bool operator==(const IteratorWrapper& o) const
	{
		return *_itrPtr == *o._itrPtr;
	}
	bool operator!=(const IteratorWrapper& o) const
	{
		return *_itrPtr != *o._itrPtr;

	}
	bool operator==(const iterator_type& o) const
	{
		return *_itrPtr == o;
	}
	bool operator!=(const iterator_type& o) const
	{
		return *_itrPtr != o;

	}
	iterator_type& getItr()const
	{
		return *_itrPtr;
	}

private:
	std::unique_ptr<iterator_type> _itrPtr = nullptr;
};

template <class iterator_type>
auto wrapIterator(const iterator_type& itr)
{
	typedef typename iterator_type::value_type value_type;
	return IteratorWrapper<iterator_type, value_type>(new iterator_type(itr));
}
