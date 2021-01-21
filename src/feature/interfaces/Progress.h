#pragma once
#include <QString>

namespace Hix
{
	namespace Features
	{
		class Feature;
	}
	class Progress
	{
	public:
		Progress();
		virtual ~Progress();
		void setDisplayText(std::string text);
		QString getDisplayText();

	private:
		Hix::Features::Feature* _currentFeature;
		uint8_t _percent = 0;
		std::string _displayText;
	};
}
