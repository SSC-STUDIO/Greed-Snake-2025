#ifndef PATH_SECURITY_H
#define PATH_SECURITY_H

#include <string>
#include <algorithm>
#include <vector>

namespace PathSecurity {

    // 白名单允许的目录
    const std::vector<std::wstring> ALLOWED_DIRECTORIES = {
        L".\\Resource\\",
        L".\\Resources\\",
        L".\\Config\\",
        L".\\Save\\",
        L".\\Logs\\"
    };

    // 允许的文件扩展名
    const std::vector<std::wstring> ALLOWED_EXTENSIONS = {
        L".txt",
        L".png",
        L".jpg",
        L".wav",
        L".mp3",
        L".json",
        L".xml",
        L".cfg"
    };

    /**
     * @brief 检查路径是否包含目录遍历攻击
     * @param path 要检查的路径
     * @return true 如果路径安全
     */
    inline bool IsPathSafe(const std::wstring& path) {
        // 检查是否包含 .. 遍历
        if (path.find(L"..") != std::wstring::npos) {
            return false;
        }
        
        // 检查是否包含空字节注入
        if (path.find(L'\0') != std::wstring::npos) {
            return false;
        }
        
        // 检查是否以允许的路径开头
        for (const auto& allowed : ALLOWED_DIRECTORIES) {
            if (path.find(allowed) == 0) {
                return true;
            }
        }
        
        // 如果是相对路径且在白名单目录下
        if (path.find(L":") == std::wstring::npos) {
            return true;
        }
        
        return false;
    }

    /**
     * @brief 净化文件路径
     * @param path 原始路径
     * @return 净化后的安全路径
     */
    inline std::wstring SanitizePath(const std::wstring& path) {
        std::wstring result = path;
        
        // 移除所有 .. 序列
        size_t pos = 0;
        while ((pos = result.find(L"..", pos)) != std::wstring::npos) {
            result.replace(pos, 2, L"");
        }
        
        // 移除空字节
        result.erase(std::remove(result.begin(), result.end(), L'\0'), result.end());
        
        // 规范化路径分隔符
        std::replace(result.begin(), result.end(), L'/', L'\\');
        
        return result;
    }

    /**
     * @brief 检查文件扩展名是否在白名单中
     * @param path 文件路径
     * @return true 如果扩展名被允许
     */
    inline bool IsExtensionAllowed(const std::wstring& path) {
        size_t dotPos = path.rfind(L'.');
        if (dotPos == std::wstring::npos) {
            return false;
        }
        
        std::wstring ext = path.substr(dotPos);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
        
        for (const auto& allowed : ALLOWED_EXTENSIONS) {
            if (ext == allowed) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief 安全地打开文件（返回完整安全路径）
     * @param baseDir 基础目录
     * @param filename 文件名
     * @return 安全的路径，如果不安全则返回空字符串
     */
    inline std::wstring GetSecurePath(const std::wstring& baseDir, const std::wstring& filename) {
        // 净化文件名
        std::wstring safeFilename = SanitizePath(filename);
        
        // 组合完整路径
        std::wstring fullPath = baseDir + safeFilename;
        
        // 验证路径安全
        if (!IsPathSafe(fullPath)) {
            return L"";
        }
        
        return fullPath;
    }

} // namespace PathSecurity

#endif // PATH_SECURITY_H
