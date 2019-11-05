#pragma once
#include "Feature.h"
#include "glmodel.h"

namespace Hix
{
	namespace Features
	{
		class LayFlat : public Feature
		{
		public:
			LayFlat(const std::unordered_set<GLModel*>& selectedModels);
			void generateLayFlat();

		private:
			std::unordered_set<GLModel*> _models;
		};
	}
}