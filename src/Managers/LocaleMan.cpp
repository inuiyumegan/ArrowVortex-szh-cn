#include <Managers/LocaleMan.h>

#include <Core/StringUtils.h>
#include <System/File.h>
#include <System/Debug.h>

namespace Vortex {

Language LocaleMan::sLanguage = Language::EN_US;
Vector<LocaleMan::Entry> LocaleMan::sEntries;

// ================================================================================================
// 简单的 JSON 字符串解析器
// 从 UTF-8 JSON 中提取双引号字符串，支持 \n \t \\ \" 转义

static String parseJsonString(const char*& p)
{
	// 跳过直到找到引号
	while (*p && *p != '"') ++p;
	if (!*p) return String();
	++p; // 跳过开始的引号

	String result;
	while (*p && *p != '"')
	{
		if (*p == '\\')
		{
			++p;
			if (!*p) break;
			switch (*p)
			{
			case 'n':  result += '\n'; break;
			case 't':  result += '\t'; break;
			case 'r':  result += '\r'; break;
			case '\\': result += '\\'; break;
			case '"':  result += '"'; break;
			case '/':  result += '/'; break;
			default:
				// 未知转义，保留原字符
				result += '\\';
				result += *p;
				break;
			}
		}
		else
		{
			// 直接追加 UTF-8 字节（支持中文等多字节字符）
			result += *p;
		}
		++p;
	}
	if (*p == '"') ++p; // 跳过结尾引号
	return result;
}

// ================================================================================================
// 创建和销毁

void LocaleMan::create()
{
	sLanguage = Language::EN_US;
	sEntries.clear();
	Debug::log("LocaleMan :: initialized (EN_US)\n");
}

void LocaleMan::destroy()
{
	sEntries.clear();
}

// ================================================================================================
// 加载翻译文件

bool LocaleMan::loadLanguageFile(StringRef filepath)
{
	bool success = false;
	String content = File::getText(filepath, &success);

	if (!success)
	{
		Debug::log("LocaleMan :: could not load language file: %s\n", filepath.str());
		return false;
	}

	sEntries.clear();

	const char* p = content.str();
	String currentKey;

	while (*p)
	{
		// 跳过空白字符
		while (*p && (*p == ' ' || *p == '\r' || *p == '\n' || *p == '\t' || *p == ',')) ++p;
		if (!*p) break;

		// 跳过注释 (// ... 直到行尾)
		if (*p == '/' && *(p + 1) == '/')
		{
			while (*p && *p != '\n') ++p;
			continue;
		}

		// 遇到 } 或 ] 表示结束
		if (*p == '}' || *p == ']') { ++p; continue; }

		// 遇到 "strings" 或其他顶层键
		if (*p == '"')
		{
			String key = parseJsonString(p);

			// 跳过冒号
			while (*p && *p != ':' && *p != '"') ++p;

			if (*p == ':')
			{
				++p;
			}

			// 跳过空白
			while (*p && (*p == ' ' || *p == '\r' || *p == '\n' || *p == '\t')) ++p;

			// 如果值是 { 对象，进入嵌套（"strings": { ... }）
			if (*p == '{')
			{
				++p; // 跳过 {
				continue;
			}

			// 如果值是字符串
			if (*p == '"')
			{
				String value = parseJsonString(p);

				if (!key.empty())
				{
					// 跳过元数据键
					if (key == "language" || key == "author")
					{
						Debug::log("LocaleMan :: %s = %s\n", key.str(), value.str());
					}
					else
					{
						Entry entry;
						entry.key = std::move(key);
						entry.value = std::move(value);
						sEntries.push_back(std::move(entry));
					}
				}
				continue;
			}
		}

		++p;
	}

	Debug::log("LocaleMan :: loaded %d translation entries from %s\n",
		sEntries.size(), filepath.str());
	return !sEntries.empty();
}

bool LocaleMan::setLanguage(Language lang)
{
	const char* filename = nullptr;

	switch (lang)
	{
	case Language::ZH_CN:
		filename = "lang/zh_CN.json";
		break;
	case Language::JA_JP:
		filename = "lang/ja_JP.json";
		break;
	case Language::KO_KR:
		filename = "lang/ko_KR.json";
		break;
	default:
		// English: 不需要翻译文件
		sEntries.clear();
		sLanguage = Language::EN_US;
		Debug::log("LocaleMan :: switched to EN_US (no translation needed)\n");
		return true;
	}

	if (!filename)
		return false;

	if (loadLanguageFile(filename))
	{
		sLanguage = lang;
		Debug::log("LocaleMan :: switched to language %d\n", (int)lang);
		return true;
	}

	Debug::log("LocaleMan :: failed to load language %d, keeping current\n", (int)lang);
	return false;
}

// ================================================================================================
// 翻译查询

String LocaleMan::translate(StringRef key)
{
	for (const auto& entry : sEntries)
	{
		if (entry.key == key)
			return entry.value;
	}
	return key; // 找不到翻译则返回 key 本身
}

String LocaleMan::tr(StringRef englishText, StringRef key)
{
	// 如果是英文，直接返回原文
	if (sLanguage == Language::EN_US)
		return englishText;

	// 优先用显式 key
	String realKey = key.empty() ? englishText : key;
	String translated = translate(realKey);

	// 如果翻译结果和 key 不同，说明找到了
	if (!(translated == realKey))
		return translated;

	// 如果 key 和英文不同，再试英文原文作为 key
	if (!key.empty() && !(key == englishText))
	{
		translated = translate(englishText);
		if (!(translated == englishText))
			return translated;
	}

	return englishText; // 最终 fallback
}

Language LocaleMan::currentLanguage()
{
	return sLanguage;
}

} // namespace Vortex
