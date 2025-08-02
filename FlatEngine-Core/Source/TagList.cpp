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
			if (m_ignoreTags.count(tag) == 0)
			{
				m_ignoreTags.emplace(tag, false);
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

	void TagList::SetIgnore(std::string tag, bool b_value)
	{
		if (m_ignoreTags.count(tag) > 0)
		{
			m_ignoreTags.at(tag) = b_value;
		}

		GameObject* owner = GetObjectByID(m_ownerID);
		if (owner != nullptr && owner->GetBody() != nullptr)
		{
			owner->GetBody()->RecreateLiveBody();
		}
	}

	void TagList::ToggleIgnore(std::string tag)
	{
		if (m_ignoreTags.count(tag) > 0)
		{
			m_ignoreTags.at(tag) = !m_ignoreTags.at(tag);
		}

		GameObject* owner = GetObjectByID(m_ownerID);
		if (owner != nullptr && owner->GetBody() != nullptr)
		{
			owner->GetBody()->RecreateLiveBody();
		}
	}

	bool TagList::IgnoresTag(std::string tag)
	{
		if (m_ignoreTags.count(tag) > 0)
		{
			return m_ignoreTags.at(tag);
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

	std::map<std::string, bool> TagList::GetIgnoreTagsMap()
	{
		return m_ignoreTags;
	}

	std::vector<std::string> TagList::GetIgnoredTags()
	{
		std::vector<std::string> ignoredTags;
		for (std::pair<std::string, bool> ignoreTag : m_ignoreTags)
		{
			if (ignoreTag.second)
			{
				ignoredTags.push_back(ignoreTag.first);
			}
		}
		return ignoredTags;
	}
}