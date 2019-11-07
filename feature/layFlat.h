#pragma once
#include "Feature.h"
#include <unordered_set>

class GLModel;

namespace Hix
{
	namespace Features
	{
		class LayFlat : public Hix::Features::Feature
		{
		public:
			LayFlat(const std::unordered_set<GLModel*>& selectedModels);
			void generateLayFlat();

		private:
			std::unordered_set<GLModel*> _models;
		};
	}
}