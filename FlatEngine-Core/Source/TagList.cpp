#include "TagList.h"
#include "FlatEngine.h"
#include "GameLoop.h"
#include "Scene.h"

#include <map>


namespace FlatEngine 
{
	std::vector<std::string> F_TagsAvailable = std::vector<std::string>();

	TagList::TagList()
	{
		m_ownerID = -1;
		UpdateAvailableTags();
		m_categoryBits = 0;
		m_maskBits = 0;
	}

	TagList::TagList(long ownerID)
	{
		m_ownerID = ownerID;
		UpdateAvailableTags();
		m_categoryBits = 0;
		m_maskBits = 0;
	}

	TagList::TagList(TagList* toCopy)
	{
		std::map<std::string, bool>::iterator iterator;
		for (iterator = toCopy->m_tags.begin(); iterator != toCopy->m_tags.end(); iterator++)
		{
			if (m_tags.count(iterator->first) > 0)
			{
				m_tags.at(iterator->first) = iterator->second;
			}
			else
			{
				m_tags.emplace(iterator->first, iterator->second);
			}
		}

		m_categoryBits = 0;
		m_maskBits = 0;
	}

	TagList::~TagList()
	{
	}


	void TagList::SetOwnerID(long ownerID)
	{
		m_ownerID = ownerID;
	}

	void TagList::UpdateAvailableTags()
	{
		for (std::string tag : F_TagsAvailable)
		{
			if (m_tags.count(tag) == 0)
			{
				m_tags.emplace(tag, false);
			}
			if (m_collidesTags.count(tag) == 0)
			{
				m_collidesTags.emplace(tag, false);
			}
		}
	}

	void TagList::SetTag(std::string tag, bool b_value)
	{
		if (m_tags.count(tag) > 0)
		{
			m_tags.at(tag) = b_value;
		}

		GameObject* owner = GetObjectByID(m_ownerID);
		if (owner != nullptr && owner->GetBody() != nullptr)
		{
			owner->GetBody()->RecreateLiveBody();
		}
	}

	void TagList::ToggleTag(std::string tag)
	{
		if (m_tags.count(tag) > 0)
		{
			m_tags.at(tag) = !m_tags.at(tag);
		}

		GameObject* owner = GetObjectByID(m_ownerID);
		if (owner != nullptr && owner->GetBody() != nullptr)
		{
			owner->GetBody()->RecreateLiveBody();
		}
	}

	bool TagList::HasTag(std::string tag)
	{
		if (m_tags.count(tag) > 0)
		{
			return m_tags.at(tag);
		}
		else
		{
			return false;
		}
	}

	void TagList::SetCollides(std::string tag, bool b_value)
	{
		if (m_collidesTags.count(tag) > 0)
		{
			m_collidesTags.at(tag) = b_value;
		}

		GameObject* owner = GetObjectByID(m_ownerID);
		if (owner != nullptr && owner->GetBody() != nullptr)
		{
			owner->GetBody()->RecreateLiveBody();
		}
	}

	void TagList::ToggleCollides(std::string tag)
	{
		if (m_collidesTags.count(tag) > 0)
		{
			m_collidesTags.at(tag) = !m_collidesTags.at(tag);
		}

		GameObject* owner = GetObjectByID(m_ownerID);
		if (owner != nullptr && owner->GetBody() != nullptr)
		{
			owner->GetBody()->RecreateLiveBody();
		}
	}

	bool TagList::CollidesTag(std::string tag)
	{
		if (m_collidesTags.count(tag) > 0)
		{
			return m_collidesTags.at(tag);
		}
		else 
		{
			return false;
		}
	}

	void TagList::CreateNewTag(std::string tagName, bool b_value)
	{
		m_tags.emplace(tagName, b_value);
	}

	void TagList::RemoveTag(std::string tagName)
	{
		if (m_tags.count(tagName) > 0)
		{
			m_tags.erase(tagName);
		}
	}

	std::map<std::string, bool> TagList::GetTagsMap()
	{
		return m_tags;
	}

	std::map<std::string, bool> TagList::GetCollidesTagsMap()
	{
		return m_collidesTags;
	}

	std::vector<std::string> TagList::GetCollidesTags()
	{
		std::vector<std::string> ignoredTags;
		for (std::pair<std::string, bool> ignoreTag : m_collidesTags)
		{
			if (ignoreTag.second)
			{
				ignoredTags.push_back(ignoreTag.first);
			}
		}
		return ignoredTags;
	}

	void TagList::SetCategoryBits(uint64_t categoryBits)
	{
		m_categoryBits = categoryBits;
	}

	uint64_t TagList::GetCategoryBits()
	{
		return m_categoryBits;
	}

	void TagList::SetMaskBits(uint64_t maskBits)
	{
		m_maskBits = maskBits;
	}

	uint64_t TagList::GetMaskBits()
	{
		return m_maskBits;
	}

	void TagList::UpdateBits()
	{
		m_categoryBits = 0;
		m_maskBits = 0;

		for (std::map<std::string, bool>::iterator iterator = m_tags.begin(); iterator != m_tags.end(); iterator++)
		{
			if (iterator->second)
			{
				uint64_t bit = 1;
				int index = (int)std::distance(m_tags.begin(), iterator);

				for (int i = 0; i < index; i++)
				{
					bit *= 2;
				}
				if (index == 0)
				{
					bit = 2;
				}

				m_categoryBits |= bit;
			}
		}		

		// Masked tags		
		for (std::map<std::string, bool>::iterator iterator = m_collidesTags.begin(); iterator != m_collidesTags.end(); iterator++)
		{
			if (iterator->second)
			{
				uint64_t bit = 1;
				int index = (int)std::distance(m_collidesTags.begin(), iterator);

				for (int i = 0; i < index; i++)
				{
					bit *= 2;
				}
				if (index == 0)
				{
					bit = 2;
				}

				m_maskBits |= bit;
			}
		}		
	}
}