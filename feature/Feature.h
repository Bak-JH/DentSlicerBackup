#pragma once

namespace Hix
{
	namespace Features
	{


		class Feature
		{
		public:
			virtual ~Feature();
		};

		template<typename FeatureType>
		bool isActive(const Feature* curr)
		{
			return dynamic_cast<FeatureType*>(curr) != nullptr;
		}
	}
}
