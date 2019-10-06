#pragma once


template <typename ItrType, typename OwnerType>
class RandomAccessIteratorBase
{

public:
	RandomAccessIteratorBase() :_index(0), _owner(nullptr)
	{}
	RandomAccessIteratorBase(size_t idx, OwnerType* owner) : _index(idx), _owner(owner)
	{}
	RandomAccessIteratorBase(const RandomAccessIteratorBase& o) : _index(o._index), _owner(o._owner)
	{
	}
	//~RandomAccessIteratorBase()
	//{}
	size_t index()const
	{
		return _index;
	}
	ItrType& operator=(const ItrType& o)
	{
		_index = o._index;
		_owner = o._owner;
		return *static_cast<ItrType>(this);;
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
		return *static_cast<ItrType*>(this);
	}
	ItrType& operator--()
	{
		--_index;
		return *static_cast<ItrType*>(this);
	}
	//post
	ItrType operator++(int)
	{
		auto tmp = *static_cast<ItrType*>(this);
		++tmp._index;
		return tmp;
	}
	ItrType operator--(int)
	{
		auto tmp = *static_cast<ItrType*>(this);
		--tmp._index;
		return tmp;

	}
	ItrType& operator+=(size_t offset)
	{
		_index += offset;
		return *static_cast<ItrType*>(this);
	}
	ItrType& operator-=(size_t offset)
	{
		_index -= offset;

		return *static_cast<ItrType*>(this);
	}
	ItrType operator+(size_t offset) const
	{
		ItrType tmp = *static_cast<const ItrType*>(this);
		return tmp += offset;
	}
	friend ItrType operator+(size_t offset, const ItrType& itr)
	{
		return itr += offset;

	}
	ItrType operator-(size_t offset) const
	{
		auto tmp = *static_cast<const ItrType*>(this);
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
	//friend struct std::hash<RandomAccessIteratorBase<ItrType, OwnerType>>;

};


//
//namespace std
//{
//	template<class ItrType, class OwnerType>
//	struct hash<RandomAccessIteratorBase<ItrType, OwnerType>>
//	{
//		//2D only!
//		std::size_t operator()(const RandomAccessIteratorBase<ItrType, OwnerType>& itr)const
//		{
//			return itr._index;
//		}
//	};
//}
//
