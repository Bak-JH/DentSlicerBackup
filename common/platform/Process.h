#pragma once
#include <string>
namespace Hix
{
	namespace Common
	{
		namespace Process
		{
			class Process
			{
			public:
				//copy impossible since process is unique
				Process(Process const&) = delete;
				Process& operator=(Process const&) = delete;

				Process(Process&& o);
				Process& operator=(Process&& o);

				~Process();
				Process();
				//windows
				Process(void* winProc);

			private:
				//windows
				void* _proc;

			};

			Process createProcessAsync(const std::wstring& program, const std::wstring& args);
		}
	}
}