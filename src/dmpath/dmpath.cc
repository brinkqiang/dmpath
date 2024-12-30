
#include "dmpath.h"
#include "dmlog.hpp"

#ifdef _WIN32
bool IsRunAsAdmin() {
	BOOL isAdmin = FALSE;
	PSID adminGroup = nullptr;

	// 分配并初始化一个SID用于管理员组
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
		// 检查当前进程是否在管理员组中
		if (!CheckTokenMembership(nullptr, adminGroup, &isAdmin)) {
			isAdmin = FALSE;
		}
		FreeSid(adminGroup);
	}

	return isAdmin == TRUE;
}

bool AddToPath(const std::string& newPath)
{
	HKEY hKey;
	LONG result;

	// 打开注册表项（用户级别）
	result = RegOpenKeyEx(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey);
	if (result != ERROR_SUCCESS) {
		std::cerr << "Error opening registry key. Error code: " << result << std::endl;
		return false;
	}

	// 获取现有的 PATH
	char currentPath[8192] = { 0 };
	DWORD bufferSize = sizeof(currentPath);
	result = RegQueryValueEx(hKey, "Path", NULL, NULL, (LPBYTE)currentPath, &bufferSize);

	if (result == ERROR_SUCCESS) {
		// 将现有 PATH 拆分为 vector
		std::stringstream ss(currentPath);
		std::string item;
		std::vector<std::string> paths;
		while (std::getline(ss, item, ';')) {
			if (!item.empty()) {
				paths.push_back(item);
			}
		}

		// 检查是否已包含新路径
		for (const auto& path : paths) {
			if (path == newPath) {
				std::cerr << "Path already exists: " << newPath << std::endl;
				RegCloseKey(hKey);
				return false;
			}
		}

		// 在现有 PATH 后添加新的路径
		std::string updatedPath = currentPath;
		if (!updatedPath.empty() && updatedPath.back() != ';') {
			updatedPath += ";";
		}
		updatedPath += newPath;

		// 写入更新后的 PATH 值
		result = RegSetValueEx(hKey, "Path", 0, REG_EXPAND_SZ, (const BYTE*)updatedPath.c_str(), updatedPath.size() + 1);

		if (result == ERROR_SUCCESS) {
			std::cout << "Successfully updated PATH." << std::endl;
		}
		else {
			std::cerr << "Error updating PATH in registry. Error code: " << result << std::endl;
			RegCloseKey(hKey);
			return false;
		}
	}
	else {
		std::cerr << "Error reading PATH from registry. Error code: " << result << std::endl;
		RegCloseKey(hKey);
		return false;
	}

	// 关闭注册表
	RegCloseKey(hKey);

	// 广播环境变量更改消息
	SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, nullptr);

	return true;
}


bool RemoveFromPath(const std::string& pathToRemove) {
	HKEY hKey;
	LONG result;

	// 打开注册表项（用户级别）
	result = RegOpenKeyEx(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey);
	if (result != ERROR_SUCCESS) {
		std::cerr << "Error opening registry key. Error code: " << result << std::endl;
		return false;
	}

	// 获取现有的 PATH
	char currentPath[8192] = { 0 };
	DWORD bufferSize = sizeof(currentPath);
	result = RegQueryValueEx(hKey, "Path", NULL, NULL, (LPBYTE)currentPath, &bufferSize);

	if (result == ERROR_SUCCESS) {
		// 将现有 PATH 拆分为 vector
		std::stringstream ss(currentPath);
		std::string item;
		std::vector<std::string> paths;
		while (std::getline(ss, item, ';')) {
			if (!item.empty()) {
				paths.push_back(item);
			}
		}

		// 检查是否存在要移除的路径
		bool pathFound = false;
		for (auto it = paths.begin(); it != paths.end();) {
			if (*it == pathToRemove) {
				it = paths.erase(it); // 移除路径
				pathFound = true;
			}
			else {
				++it;
			}
		}

		if (!pathFound) {
			std::cerr << "Path not found: " << pathToRemove << std::endl;
			RegCloseKey(hKey);
			return false;
		}

		// 将剩余的路径重新拼接为字符串
		std::string updatedPath;
		for (const auto& path : paths) {
			if (!updatedPath.empty()) {
				updatedPath += ";";
			}
			updatedPath += path;
		}

		// 写入更新后的 PATH 值
		result = RegSetValueEx(hKey, "Path", 0, REG_EXPAND_SZ, (const BYTE*)updatedPath.c_str(), updatedPath.size() + 1);

		if (result == ERROR_SUCCESS) {
			std::cout << "Successfully removed path from PATH." << std::endl;
		}
		else {
			std::cerr << "Error updating PATH in registry. Error code: " << result << std::endl;
			RegCloseKey(hKey);
			return false;
		}
	}
	else {
		std::cerr << "Error reading PATH from registry. Error code: " << result << std::endl;
		RegCloseKey(hKey);
		return false;
	}

	// 关闭注册表
	RegCloseKey(hKey);

	// 广播环境变量更改消息
	SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, nullptr);

	return true;
}
#else

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
bool AddToPath(const std::string& newPath) {
    const char* home = getenv("HOME");
    if (!home) {
        std::cerr << "Error: HOME environment variable not set." << std::endl;
        return false;
    }

    std::string shellConfig = std::string(home) + "/.bashrc";
    struct stat buffer;
    if (stat((std::string(home) + "/.zshrc").c_str(), &buffer) == 0) {
        shellConfig = std::string(home) + "/.zshrc"; // 优先支持 zsh
    }

    // 读取现有的 PATH
    std::ifstream configFile(shellConfig);
    if (!configFile) {
        std::cerr << "Error: Unable to open shell configuration file: " << shellConfig << std::endl;
        return false;
    }

    std::ostringstream configContent;
    configContent << configFile.rdbuf();
    configFile.close();

    std::string content = configContent.str();
    std::string exportPathLine = fmt::format("export PATH=\"$PATH:{}\" ## Added by AddToPath function\n", newPath);

    // 检查 PATH 是否已包含新路径
    if (content.find(newPath) != std::string::npos) {
        std::cerr << "Path already exists in shell configuration: " << newPath << std::endl;
        return false;
    }

    // 添加新路径
    std::ofstream configFileOut(shellConfig, std::ios::app);
    if (!configFileOut) {
        std::cerr << "Error: Unable to write to shell configuration file: " << shellConfig << std::endl;
        return false;
    }

    configFileOut << exportPathLine;

    configFileOut.close();

    std::cout << "Successfully added path. Please run 'source " << shellConfig << "' or restart your shell to apply changes." << std::endl;
    return true;
}

bool RemoveFromPath(const std::string& pathToRemove) {
    const char* home = getenv("HOME");
    if (!home) {
        std::cerr << "Error: HOME environment variable not set." << std::endl;
        return false;
    }

    std::string shellConfig = std::string(home) + "/.bashrc";
    struct stat buffer;
    if (stat((std::string(home) + "/.zshrc").c_str(), &buffer) == 0) {
        shellConfig = std::string(home) + "/.zshrc"; // 优先支持 zsh
    }

    // 读取现有的配置文件内容
    std::ifstream configFile(shellConfig);
    if (!configFile) {
        std::cerr << "Error: Unable to open shell configuration file: " << shellConfig << std::endl;
        return false;
    }

    std::ostringstream configContent;
    configContent << configFile.rdbuf();
    configFile.close();

    std::string content = configContent.str();
    std::string exportPathLine = fmt::format("export PATH=\"$PATH:{}\" ## Added by AddToPath function\n", pathToRemove);


    size_t startPos = content.find(exportPathLine);

    if (startPos == std::string::npos) {
		std::cerr << "Path not found in shell configuration: " << pathToRemove << std::endl;
		return false;
    }

    // 删除目标路径
    size_t endPos = startPos + exportPathLine.length();

    content.erase(startPos, endPos - startPos);

    // 写入更新后的配置文件
    std::ofstream configFileOut(shellConfig);
    if (!configFileOut) {
        std::cerr << "Error: Unable to write to shell configuration file: " << shellConfig << std::endl;
        return false;
    }

    configFileOut << content;
    configFileOut.close();

    std::cout << "Successfully removed path and related comments. Please run 'source " << shellConfig << "' or restart your shell to apply changes." << std::endl;
    return true;
}
#endif