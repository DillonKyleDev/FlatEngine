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
		void SetCollides(std::string tag, bool b_value);
		void ToggleCollides(std::string tag);
		bool CollidesTag(std::string);
		void CreateNewTag(std::string tagName, bool b_value = false);
		void RemoveTag(std::string tagName);
		std::map<std::string, bool> GetTagsMap();
		std::map<std::string, bool> GetCollidesTagsMap();
		std::vector<std::string> GetCollidesTags();
		
	private:
		std::map<std::string, bool> m_tags;
		std::map<std::string, bool> m_collidesTags;
		long m_ownerID;
	};

}




