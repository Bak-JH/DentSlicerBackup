#include "IndexedList.h"
#include <vector>
#include <set>
#include <numeric>
#include <variant>

template <class T, class A = std::allocator<T>>
class TrackedIndexedList : public IndexedList<T, A> {

	typedef typename IndexedList<T, A> parent_type;
public:
	typedef typename std::variant<bool, std::set<size_t>> changes_type;
	typedef typename parent_type::iterator iterator;
	typedef typename parent_type::const_iterator const_iterator;

	TrackedIndexedList()
	{

	}
	TrackedIndexedList(const TrackedIndexedList& o) :parent_type(o)
	{
	}
	TrackedIndexedList(std::initializer_list<T> iniList) : TrackedIndexedList(iniList, std::allocator<T>())
	{
	}

	TrackedIndexedList(std::initializer_list<T> iniList, const parent_type::allocator_type& allocator) :parent_type(iniList, allocator)
	{
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
			std::set<size_t> changes = std::move(_changedIndices);
			clearChanges();
			return changes;
		}

	}
	//when you want to mark change in explicit manner
	void markChanged(const_iterator itr)
	{
		markChanged(itr - cbegin());
	}
	void markChanged(size_t index)
	{
		_changedIndices.insert(index);

	}

protected:
	void addedElement(size_t index) override
	{
		if (index == 0)
			_changedAll = true;
		else
			_changedIndices.insert(index);
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
		_changedIndices.insert(index);
	}
private:
	inline void clearChanges()
	{
		_changedAll = false;
		_changedIndices.clear();
	}
	bool _changedAll = false;
	std::set<size_t> _changedIndices;
};