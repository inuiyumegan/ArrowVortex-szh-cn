#pragma once

#include <Core/String.h>
#include <Core/Vector.h>

namespace Vortex {

/// Language enum
enum class Language
{
	EN_US,  // English (no translation file needed)
	ZH_CN,  // Simplified Chinese
	JA_JP,  // Japanese
	KO_KR,  // Korean
	NUM_LANGUAGES
};

/// Translation manager (singleton pattern)
/// Loads key-value pairs from a JSON translation file and provides runtime translation queries.
struct LocaleMan
{
	static void create();
	static void destroy();

	/// Set the current language and load the corresponding translation file
	static bool setLanguage(Language lang);

	/// Load translation file directly by path
	static bool loadLanguageFile(StringRef filepath);

	/// Translate: look up by key, return the key itself if not found
	static String translate(StringRef key);

	/// Convenience translation: return translation if found, otherwise return English source text
	/// @param englishText  English source text (used as fallback)
	/// @param key          Translation key (optional; if empty, englishText is used as key)
	static String tr(StringRef englishText, StringRef key = String());

	/// Return the current language
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

/// Global convenience macros
/// _TR(en)         - Single param: translate using English text as key
/// _TR(en, key)    - Two params: translate using key, en as fallback
/// _T(key)         - Direct key translation
#define _TR_1(en)           Vortex::LocaleMan::tr(en)
#define _TR_2(en, key)      Vortex::LocaleMan::tr(en, key)
#define _TR_GET(_1, _2, NAME, ...) NAME
#define _TR(...)            _TR_GET(__VA_ARGS__, _TR_2, _TR_1)(__VA_ARGS__)
#define _T(key)             Vortex::LocaleMan::translate(key)

} // namespace Vortex
