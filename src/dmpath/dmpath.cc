
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
#endif