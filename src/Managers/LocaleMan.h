#pragma once

#include <Core/String.h>
#include <Core/Vector.h>

namespace Vortex {

/// 语言枚举
enum class Language
{
	EN_US,  // English (no translation file needed)
	ZH_CN,  // 简体中文
	JA_JP,  // 日本語
	KO_KR,  // 한국어
	NUM_LANGUAGES
};

/// 翻译管理器（单例模式）
/// 从 JSON 格式的翻译文件加载键值对，提供运行时翻译查询。
struct LocaleMan
{
	static void create();
	static void destroy();

	/// 设置当前语言并加载对应的翻译文件
	static bool setLanguage(Language lang);

	/// 通过语言文件路径直接加载
	static bool loadLanguageFile(StringRef filepath);

	/// 翻译：按 key 查找翻译，找不到则返回 key 本身
	static String translate(StringRef key);

	/// 便捷翻译：有翻译用翻译，无翻译返回英文原文
	/// @param englishText  英文原文（作为 fallback）
	/// @param key          翻译键（可选，为空则用 englishText 作为 key）
	static String tr(StringRef englishText, StringRef key = String());

	/// 返回当前语言
	static Language currentLanguage();

private:
	LocaleMan() {}

	struct Entry
	{
		String key;
		String value;
	};

	static Language sLanguage;
	static Vector<Entry> sEntries;
};

/// 全局便捷宏
/// _TR(en, key)   - 带显式key的翻译
/// _T(key)        - 直接key翻译
#define _TR(en, key) Vortex::LocaleMan::tr(en, key)
#define _T(key)      Vortex::LocaleMan::translate(key)

} // namespace Vortex
