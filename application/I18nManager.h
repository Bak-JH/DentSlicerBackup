#pragma once
#include <string>
#include <spirit_po/spirit_po.hpp>



//shorthand for translation HixTrans
std::string htr();

namespace Hix
{

	namespace Application
	{
		class ApplicationManager;

		class I18nManager
		{
		public:
			I18nManager();
			spirit_po::default_catalog& cat();
		private:
			spirit_po::default_catalog _cat;

		};


		class I18nManagerLoader
		{
		private:
			static void init(I18nManager& manager);
			friend class Hix::Application::ApplicationManager;
		};

	}
}
