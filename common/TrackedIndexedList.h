#pragma once
#include "IndexedList.h"
#include <vector>
#include <unordered_set>
#include <numeric>
#include <variant>



template <class T, class A = std::allocator<T>, class ItrFac = IndexedListItr::DefaultIteratorFactory<T, A>>
class TrackedIndexedList : public IndexedList<T, A, ItrFac> {

    typedef IndexedList<T, A, ItrFac> parent_type;
public:
	typedef typename std::variant<bool, std::unordered_set<size_t>> changes_type;
	typedef typename parent_type::iterator iterator;
	typedef typename parent_type::const_iterator const_iterator;

	TrackedIndexedList(ItrFac itrFac = ItrFac()): parent_type(itrFac)
	{
	}
	TrackedIndexedList(const TrackedIndexedList& o) :parent_type(o)
	{
		_changedAll = true;
	}
	TrackedIndexedList(std::initializer_list<T> iniList, ItrFac itrFac = ItrFac()):
		TrackedIndexedList(iniList, std::allocator<T>(), itrFac)
	{
		_changedAll = true;
	}

    TrackedIndexedList(std::initializer_list<T> iniList, const typename parent_type::allocator_type& allocator,
		ItrFac itrFac = ItrFac()) :parent_type(iniList, allocator, itrFac)
	{
		_changedAll = true;
	}


	virtual ~TrackedIndexedList()
	{}

	TrackedIndexedList& operator=(const TrackedIndexedList& o)
	{
		parent_type::operator=(o);
		return *this;
	}

	TrackedIndexedList& operator=(std::initializer_list<T> iniList) { // assign initializer_list
		assign(iniList.begin(), iniList.end());
		return *this;
	}
	changes_type flushChanges()
	{
		if (_changedAll)
		{
			clearChanges();
			return true;
		}
		else
		{
			std::unordered_set<size_t> changes = std::move(_changedIndices);
			clearChanges();
			return changes;
		}

	}
	//when you want to mark change in explicit manner
	void markChanged(const_iterator itr)
	{
		markChanged(itr - parent_type::cbegin());
	}
	void markChanged(size_t index)
	{
		_changedIndices.insert(index);

	}
	void markChangedAll()
	{
		_changedAll = true;
	}

protected:
	void addedElement(size_t index) override
	{
		if (index == 0)
			_changedAll = true;
		else
		{
			if (!_changedAll)
				_changedIndices.insert(index);
		}
	}
	void addedAll() override
	{
		_changedAll = true;
	}
	void removedElement(size_t index) override
	{
		//we do not record changes that are externally visible via size();
		if (index == 0)
			_changedAll = true;
	}
	void removedAll() override
	{
		_changedAll = true;
	}
	void modifiedElement(size_t index) override
	{
		if (!_changedAll)
			_changedIndices.insert(index);
	}
private:
	inline void clearChanges()
	{
		_changedAll = false;
		_changedIndices.clear();
	}
	bool _changedAll = false;
	std::unordered_set<size_t> _changedIndices;
};
