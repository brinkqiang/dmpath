
#include "dmpath.h"

#ifdef _WIN32

bool AddToPath(const std::string& newPath)
{
	HKEY hKey;
	LONG result;

	// ��ע�����û�����
	result = RegOpenKeyEx(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey);
	if (result != ERROR_SUCCESS) {
		std::cerr << "Error opening registry key. Error code: " << result << std::endl;
		return false;
	}

	// ��ȡ���е� PATH
	char currentPath[8192] = { 0 };
	DWORD bufferSize = sizeof(currentPath);
	result = RegQueryValueEx(hKey, "Path", NULL, NULL, (LPBYTE)currentPath, &bufferSize);

	if (result == ERROR_SUCCESS) {
		// ������ PATH ���Ϊ vector
		std::stringstream ss(currentPath);
		std::string item;
		std::vector<std::string> paths;
		while (std::getline(ss, item, ';')) {
			if (!item.empty()) {
				paths.push_back(item);
			}
		}

		// ����Ƿ��Ѱ�����·��
		for (const auto& path : paths) {
			if (path == newPath) {
				std::cerr << "Path already exists: " << newPath << std::endl;
				RegCloseKey(hKey);
				return false;
			}
		}

		// ������ PATH ������µ�·��
		std::string updatedPath = currentPath;
		if (!updatedPath.empty() && updatedPath.back() != ';') {
			updatedPath += ";";
		}
		updatedPath += newPath;

		// д����º�� PATH ֵ
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

	// �ر�ע���
	RegCloseKey(hKey);

	// �㲥��������������Ϣ
	SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, nullptr);

	return true;
}


bool RemoveFromPath(const std::string& pathToRemove) {
	HKEY hKey;
	LONG result;

	// ��ע�����û�����
	result = RegOpenKeyEx(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey);
	if (result != ERROR_SUCCESS) {
		std::cerr << "Error opening registry key. Error code: " << result << std::endl;
		return false;
	}

	// ��ȡ���е� PATH
	char currentPath[8192] = { 0 };
	DWORD bufferSize = sizeof(currentPath);
	result = RegQueryValueEx(hKey, "Path", NULL, NULL, (LPBYTE)currentPath, &bufferSize);

	if (result == ERROR_SUCCESS) {
		// ������ PATH ���Ϊ vector
		std::stringstream ss(currentPath);
		std::string item;
		std::vector<std::string> paths;
		while (std::getline(ss, item, ';')) {
			if (!item.empty()) {
				paths.push_back(item);
			}
		}

		// ����Ƿ����Ҫ�Ƴ���·��
		bool pathFound = false;
		for (auto it = paths.begin(); it != paths.end();) {
			if (*it == pathToRemove) {
				it = paths.erase(it); // �Ƴ�·��
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

		// ��ʣ���·������ƴ��Ϊ�ַ���
		std::string updatedPath;
		for (const auto& path : paths) {
			if (!updatedPath.empty()) {
				updatedPath += ";";
			}
			updatedPath += path;
		}

		// д����º�� PATH ֵ
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

	// �ر�ע���
	RegCloseKey(hKey);

	// �㲥��������������Ϣ
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
        shellConfig = std::string(home) + "/.zshrc"; // ����֧�� zsh
    }

    // ��ȡ���е� PATH
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

    // ��� PATH �Ƿ��Ѱ�����·��
    if (content.find(newPath) != std::string::npos) {
        std::cerr << "Path already exists in shell configuration: " << newPath << std::endl;
        return false;
    }

    // �����·��
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
        shellConfig = std::string(home) + "/.zshrc"; // ����֧�� zsh
    }

    // ��ȡ���е� PATH
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

    // ���·���Ƿ����
    if (pathPos == std::string::npos) {
        std::cerr << "Path not found in shell configuration: " << pathToRemove << std::endl;
        return false;
    }

    // �Ƴ�·��
    std::string newContent = content;
    newContent.erase(pathPos, pathToRemove.length());

    // д����º���ļ�
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