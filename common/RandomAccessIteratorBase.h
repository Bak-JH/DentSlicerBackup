#pragma once


template <typename ItrType, typename OwnerType>
class RandomAccessIteratorBase
{

public:
	RandomAccessIteratorBase() :_index(0), _onwer(nullptr)
	{}
	RandomAccessIteratorBase(size_t idx, OwnerType* owner) : _index(idx), _onwer(owner)
	{}
	size_t index()const
	{
		return _index;
	}
	ItrType& operator=(const ItrType& o)
	{
		_index = o._index;
		_owner = o._owner;
		return *this;
	}
	bool initialized()const
	{
		return _owner != nullptr;
	}
	bool operator==(const ItrType& o) const
	{
		return _index == o._index;
	}
	bool operator!=(const ItrType& o) const
	{
		return !operator==(o);
	}

	//random access ItrType
	bool operator< (const ItrType& o) const
	{
		return _index < o._index;
	}
	bool operator> (const ItrType& o) const
	{
		return _index > o._index;
	}
	bool operator<=(const ItrType& o) const
	{
		return _index <= o._index;
	}
	bool operator>=(const ItrType& o) const
	{
		return _index >= o._index;
	} //optional

	//pre
	ItrType& operator++()
	{
		++_index;
		return *this;
	}
	ItrType& operator--()
	{
		--_index;
		return *this;
	}
	//post
	ItrType operator++(int)
	{
		auto tmp = *this;
		++_index;
		return tmp;
	}
	ItrType operator--(int)
	{
		auto tmp = *this;
		--_index;
		return tmp;

	}
	ItrType& operator+=(size_t offset)
	{
		_index += offset;
		return *this;
	}
	ItrType& operator-=(size_t offset)
	{
		_index -= offset;

		return *this;
	}
	ItrType operator+(size_t offset) const
	{
		auto tmp = *this;
		return tmp += offset;
	}
	friend ItrType operator+(size_t offset, const ItrType& itr)
	{
		return itr += offset;

	}
	ItrType operator-(size_t offset) const
	{
		auto tmp = *this;
		return tmp -= offset;
	}
	size_t operator-(const ItrType& itr)const
	{
		return _index - itr._index;
	}
	//notice distinct lack of -> and * and other dereference/accesse methods.
	//they should be implemented in children classes.

protected:
	OwnerType* _owner;
	size_t _index;
private:
	friend struct std::hash<RandomAccessIteratorBase>;

};



namespace std
{
	template<class ItrType, class OwnerType>
	struct hash<RandomAccessIteratorBase<ItrType, OwnerType>>
	{
		//2D only!
		std::size_t operator()(const RandomAccessIteratorBase<ItrType, OwnerType>& itr)const
		{
			return itr._index;
		}
	};
}

