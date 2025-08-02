#pragma once
#include <vector>
#include <map>
#include <string>


namespace FlatEngine
{
	class TagList
	{
	public:
		TagList();
		TagList(long owner);
		TagList(TagList* toCopy);
		~TagList();

		void UpdateAvailableTags();
		void SetTag(std::string tag, bool b_value);
		void ToggleTag(std::string tag);
		bool HasTag(std::string);
		void SetIgnore(std::string tag, bool b_value);
		void ToggleIgnore(std::string tag);
		bool IgnoresTag(std::string);
		void CreateNewTag(std::string tagName, bool b_value = false);
		void RemoveTag(std::string tagName);
		std::map<std::string, bool> GetTagsMap();
		std::map<std::string, bool> GetIgnoreTagsMap();
		std::vector<std::string> GetIgnoredTags();
		
	private:
		std::map<std::string, bool> m_tags;
		std::map<std::string, bool> m_ignoreTags;
		long m_ownerID;
	};

}




