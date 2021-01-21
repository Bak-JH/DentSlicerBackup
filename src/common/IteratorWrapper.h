#pragma once
#include <memory>

//don't use traits here as that may create circular dependency
template <class iterator_type>
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

    auto operator*() const
	{
		return **_itrPtr;
	}
    auto operator->() const
	{
        return _itrPtr->operator->();

	}
    auto toPtr() const
    {
        return _itrPtr->operator->();
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
    return IteratorWrapper<iterator_type>(new iterator_type(itr));
}
