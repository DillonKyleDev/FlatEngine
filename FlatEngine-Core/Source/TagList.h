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

		void SetOwnerID(long ownerID);
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
		void SetCategoryBits(uint64_t categoryBits);
		uint64_t GetCategoryBits();
		void SetMaskBits(uint64_t maskBits);
		uint64_t GetMaskBits();
		void UpdateBits();
		
	private:
		std::map<std::string, bool> m_tags;
		std::map<std::string, bool> m_collidesTags;
		long m_ownerID;
		uint64_t m_categoryBits;
		uint64_t m_maskBits;
	};

}




