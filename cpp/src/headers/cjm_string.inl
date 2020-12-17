#ifndef cjm_string_inl
#define cjm_string_inl
#include <algorithm>
#include <locale>
#include "cjm_string.hpp"

template<typename Char, typename CharTraits, typename Allocator>
std::basic_string<Char, CharTraits, Allocator>
cjm::string::trim(std::basic_string<Char, CharTraits, Allocator>&& trimMe)
{
	using str = std::basic_string<Char, CharTraits, Allocator>;
	str toBeTrimmed(std::move(trimMe));
	string_helper::ltrim(toBeTrimmed);
	string_helper::rtrim(toBeTrimmed);
	return toBeTrimmed;
}

template <typename Char, typename CharTraits, typename Allocator>
std::basic_string<Char, CharTraits, Allocator> cjm::string::
trim(const std::basic_string<Char, CharTraits, Allocator>& trimMe)
{
	using str = std::basic_string<Char, CharTraits, Allocator>;
	str copy = trimMe;
	return trim(std::move(copy));
}

template <typename Char, typename CharTraits, typename Allocator>
void cjm::string::string_helper::ltrim(std::basic_string<Char, CharTraits, Allocator>& toBeLTrimmed)
{
	auto it = std::find_if(toBeLTrimmed.begin(), toBeLTrimmed.end(), [](Char c) -> bool
	{
		return !std::isspace<Char>(c, std::locale::classic());
	});
	toBeLTrimmed.erase(toBeLTrimmed.begin(), it);
}
template<typename Char, typename CharTraits, typename Allocator>
void cjm::string::string_helper::rtrim(std::basic_string<Char, CharTraits, Allocator>& toBeRTrimmed)
{
	auto it = std::find_if(toBeRTrimmed.rbegin(), toBeRTrimmed.rend(), [](Char c) -> bool
	{
		return !std::isspace<Char>(c, std::locale::classic());
	});
	toBeRTrimmed.erase(it.base(), toBeRTrimmed.end());
}
#endif 
 