#pragma once
#include <Windows.h>
#include <taskschd.h>
#include <wrl/client.h>
#include <comdef.h>
#include <Lmcons.h>
#include <filesystem>
#pragma comment(lib, "taskschd.lib")

namespace StartupMigration
{
    inline bool RemoveLegacyTask()
    {
        const HRESULT initialized = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        struct ComScope { bool owned; ~ComScope() { if (owned) CoUninitialize(); } } scope{SUCCEEDED(initialized)};
        if (FAILED(initialized) && initialized != RPC_E_CHANGED_MODE) return false;
        using Microsoft::WRL::ComPtr;
        ComPtr<ITaskService> service;
        if (FAILED(CoCreateInstance(CLSID_TaskScheduler, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&service))) ||
            FAILED(service->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t()))) return false;
        ComPtr<ITaskFolder> folder;
        ComPtr<IRegisteredTask> task;
        ComPtr<ITaskDefinition> definition;
        ComPtr<IPrincipal> principal;
        if (FAILED(service->GetFolder(_bstr_t(L"\\"), &folder)) ||
            FAILED(folder->GetTask(_bstr_t(L"ThreeFingerDrag"), &task)) ||
            FAILED(task->get_Definition(&definition)) || FAILED(definition->get_Principal(&principal))) return false;
        BSTR user = nullptr;
        if (FAILED(principal->get_UserId(&user)) || !user) return false;
        std::wstring taskUser(user);
        SysFreeString(user);
        wchar_t currentUser[UNLEN + 1] = {};
        DWORD length = UNLEN + 1;
        if (!GetUserNameW(currentUser, &length)) return false;
        const auto slash = taskUser.find_last_of(L'\\');
        const auto shortName = taskUser.substr(slash == std::wstring::npos ? 0 : slash + 1);
        if (_wcsicmp(shortName.c_str(), currentUser) != 0) return false;
        ComPtr<IActionCollection> actions;
        ComPtr<IAction> action;
        ComPtr<IExecAction> execution;
        if (FAILED(definition->get_Actions(&actions)) || FAILED(actions->get_Item(1, &action)) ||
            FAILED(action.As(&execution))) return false;
        BSTR executable = nullptr;
        if (FAILED(execution->get_Path(&executable)) || !executable) return false;
        const auto filename = std::filesystem::path(executable).filename().wstring();
        SysFreeString(executable);
        if (_wcsicmp(filename.c_str(), L"ThreeFingerDrag.exe") != 0) return false;
        // Preserve startup before deleting the old task; a registry write failure
        // must leave the old user's startup preference intact.
        wchar_t currentExecutable[32768] = {};
        const DWORD size = GetModuleFileNameW(nullptr, currentExecutable, 32768);
        if (size == 0 || size >= 32768) return false;
        HKEY key = nullptr;
        if (RegCreateKeyExW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, nullptr, 0,
            KEY_SET_VALUE, nullptr, &key, nullptr) != ERROR_SUCCESS) return false;
        const std::wstring command = L"\"" + std::wstring(currentExecutable) + L"\"";
        const LONG saved = RegSetValueExW(key, L"ThreeFingerDrag", 0, REG_SZ,
            reinterpret_cast<const BYTE*>(command.c_str()),
            static_cast<DWORD>((command.size() + 1) * sizeof(wchar_t)));
        RegCloseKey(key);
        if (saved != ERROR_SUCCESS) return false;
        return SUCCEEDED(folder->DeleteTask(_bstr_t(L"ThreeFingerDrag"), 0));
    }
}
