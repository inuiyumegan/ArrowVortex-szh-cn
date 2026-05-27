#include <Managers/LocaleMan.h>

#include <Core/StringUtils.h>
#include <System/File.h>
#include <System/Debug.h>

namespace Vortex {

Language LocaleMan::sLanguage = Language::EN_US;
Vector<LocaleMan::Entry> LocaleMan::sEntries;

// ================================================================================================
// Simple JSON string parser
// Extracts double-quoted strings from UTF-8 JSON, supports \n \t \\ \" escapes

static String parseJsonString(const char*& p)
{
	// Skip until we find a quote
	while (*p && *p != '"') ++p;
	if (!*p) return String();
	++p; // Skip opening quote

	String result;
	while (*p && *p != '"')
	{
		if (*p == '\\')
		{
			++p;
			if (!*p) break;
			switch (*p)
			{
			case 'n':  Str::append(result, '\n'); break;
			case 't':  Str::append(result, '\t'); break;
			case 'r':  Str::append(result, '\r'); break;
			case '\\': Str::append(result, '\\'); break;
			case '"':  Str::append(result, '"'); break;
			case '/':  Str::append(result, '/'); break;
			default:
				// Unknown escape, keep the original characters
				Str::append(result, '\\');
				Str::append(result, *p);
				break;
			}
		}
		else
		{
			// Directly append UTF-8 bytes (supports multi-byte characters like CJK)
			Str::append(result, *p);
		}
		++p;
	}
	if (*p == '"') ++p; // Skip closing quote
	return result;
}

// ================================================================================================
// Create and destroy

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
// Load translation file

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
		// Skip whitespace characters
		while (*p && (*p == ' ' || *p == '\r' || *p == '\n' || *p == '\t' || *p == ',')) ++p;
		if (!*p) break;

		// Skip comments (// ... to end of line)
		if (*p == '/' && *(p + 1) == '/')
		{
			while (*p && *p != '\n') ++p;
			continue;
		}

		// } or ] means end of block
		if (*p == '}' || *p == ']') { ++p; continue; }

		// "strings" or other top-level keys
		if (*p == '"')
		{
			String key = parseJsonString(p);

			// Skip the colon
			while (*p && *p != ':' && *p != '"') ++p;

			if (*p == ':')
			{
				++p;
			}

			// Skip whitespace
			while (*p && (*p == ' ' || *p == '\r' || *p == '\n' || *p == '\t')) ++p;

			// If value is { object, enter nesting ("strings": { ... })
			if (*p == '{')
			{
				++p; // Skip {
				continue;
			}

			// If value is a string
			if (*p == '"')
			{
				String value = parseJsonString(p);

				if (!key.empty())
				{
					// Skip metadata keys
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
		// English: no translation file needed
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
	return false;
}

// ================================================================================================
// Translation query

String LocaleMan::translate(StringRef key)
{
	for (const auto& entry : sEntries)
	{
		if (entry.key == key)
			return entry.value;
	}
	return key; // No translation found, return key itself
}

String LocaleMan::tr(StringRef englishText, StringRef key)
{
	// English: return source text directly
	if (sLanguage == Language::EN_US)
		return englishText;

	// Prefer explicit key
	String realKey = key.empty() ? englishText : key;
	String translated = translate(realKey);

	// If translation differs from key, we found a match
	if (!(translated == realKey))
		return translated;

	// If key differs from English, retry with English as key
	if (!key.empty() && !(key == englishText))
	{
		translated = translate(englishText);
		if (!(translated == englishText))
			return translated;
	}

	return englishText; // Final fallback
}

Language LocaleMan::currentLanguage()
{
	return sLanguage;
}

} // namespace Vortex
