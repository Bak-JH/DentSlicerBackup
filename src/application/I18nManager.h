#pragma once
#include <string>
#include <optional>



namespace Hix
{

	namespace Application
	{
		class ApplicationManager;

		class I18nManager
		{
		public:
			I18nManager();
		private:

		};


		class I18nManagerLoader
		{
		private:
			static void init(I18nManager& manager);
			friend class Hix::Application::ApplicationManager;
		};

	}
}
