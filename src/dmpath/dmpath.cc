
#include "dmpath.h"

#ifdef _WIN32

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
    std::string exportPathLine = "export PATH=";

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

    configFileOut << "\n# Added by AddToPath function\n";
    configFileOut << exportPathLine << "\"$PATH:" << newPath << "\"" << std::endl;

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
    std::string exportPathLine = "export PATH=";
    std::size_t pathPos = content.find(pathToRemove);

    // 检查路径是否存在
    if (pathPos == std::string::npos) {
        std::cerr << "Path not found in shell configuration: " << pathToRemove << std::endl;
        return false;
    }

    // 移除路径
    std::string newContent = content;
    newContent.erase(pathPos, pathToRemove.length());

    // 写入更新后的文件
    std::ofstream configFileOut(shellConfig);
    if (!configFileOut) {
        std::cerr << "Error: Unable to write to shell configuration file: " << shellConfig << std::endl;
        return false;
    }

    configFileOut << newContent;
    configFileOut.close();

    std::cout << "Successfully removed path. Please run 'source " << shellConfig << "' or restart your shell to apply changes." << std::endl;
    return true;
}
#endif