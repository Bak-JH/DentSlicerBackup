#include "Process.h"
#include <windows.h>
#include <stdexcept>
using namespace Hix::Common::Process;




Hix::Common::Process::Process::Process(Process&& o): _proc(o._proc)
{
    o._proc = nullptr;
}

Process& Hix::Common::Process::Process::operator=(Process&& o)
{
    if(_proc)
        TerminateProcess(_proc, 0);
    _proc = o._proc;
    o._proc = nullptr;
    return *this;
}

Hix::Common::Process::Process::~Process()
{
    if (_proc)
        !TerminateProcess(_proc, 0);


}

Hix::Common::Process::Process::Process():_proc(nullptr)
{
}

Hix::Common::Process::Process::Process(void* winProc):_proc(winProc)
{
}



Process Hix::Common::Process::createProcessAsync(const std::wstring& program, const std::wstring& args)
{
    //_putenv("__COMPAT_LAYER=RunAsAdmin");
    //STARTUPINFO info = { sizeof(info) };
    //PROCESS_INFORMATION processInfo;
    //wchar_t modifyBuffer[4096]{};
    //wcsncpy(modifyBuffer, args.c_str(), args.size());
    //if (CreateProcess(program.c_str(), modifyBuffer, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo)) {
    //    //CloseHandle(processInfo.hProcess); // Cleanup since you don't need this
    //    //CloseHandle(processInfo.hThread); // Cleanup since you don't need this
    //    return Process(processInfo.hProcess);
    //}
    SHELLEXECUTEINFO info{};
    info.cbSize = sizeof(SHELLEXECUTEINFO);
    info.fMask = SEE_MASK_NOCLOSEPROCESS /*| SEE_MASK_NO_CONSOLE*/;
    info.hwnd = NULL;
    info.lpVerb = L"runas";
    info.lpFile = program.c_str();
    info.lpParameters = args.c_str();
    info.lpDirectory = NULL;
    info.nShow = SW_HIDE;
    //SW_HIDE
    if (ShellExecuteEx(&info))
    {
        return info.hProcess;
    }
    else
    {
        auto err = GetLastError();
        throw std::runtime_error("failed to create child process");
    }
}
