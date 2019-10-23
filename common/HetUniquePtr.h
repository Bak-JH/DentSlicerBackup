#pragma once
#include <memory>

namespace Hix
{
	namespace Memory
	{
		//a delete-or-not smart pointer to enable Heterogenous lookups.
		template<class T>
		struct ExplicitDeleter {
			explicit ExplicitDeleter(bool doDelete = true) : _delete(doDelete) {}
			void operator()(T* p) const {
				if (_delete) delete p;
			}
			bool _delete;
		};

		template<class T>
		using HetUniquePtr = std::unique_ptr<T, ExplicitDeleter<T>>;

		template<class T>
		HetUniquePtr<T> toDummy(T* raw) {
			return HetUniquePtr<T>(raw, ExplicitDeleter<T>(false));
		}

		template<class T>
		HetUniquePtr<T> toUnique(T* raw) {
			return HetUniquePtr<T>(raw, ExplicitDeleter<T>());
		}

	}
}
