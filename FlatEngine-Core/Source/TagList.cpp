#include "TagList.h"
#include "FlatEngine.h"
#include "GameLoop.h"
#include "Scene.h"


namespace FlatEngine 
{
	std::vector<std::string> F_TagsAvailable = std::vector<std::string>();

	TagList::TagList()
	{
		m_ownerID = -1;
		UpdateAvailableTags();
	}

	TagList::TagList(long ownerID)
	{
		m_ownerID = ownerID;
		UpdateAvailableTags();
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
	}

	TagList::~TagList()
	{
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
}