#pragma once



namespace Hix
{
	namespace Common
	{
		//when std::hash functor is not good enough
		template <class HashedType>
		class Hasher {
		public:
			virtual size_t operator()(const HashedType& hashed)const = 0;
		};
	}
}

