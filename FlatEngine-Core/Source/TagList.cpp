#include "managers/Assets.h"
#include "managers/SceneManager.h"
#include "TagList.h"


namespace FlatEngine 
{
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
		m_ownerID = -1;
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
		for (iterator = toCopy->m_collidesTags.begin(); iterator != toCopy->m_collidesTags.end(); iterator++)
		{
			if (m_collidesTags.count(iterator->first) > 0)
			{
				m_collidesTags.at(iterator->first) = iterator->second;
			}
			else
			{
				m_collidesTags.emplace(iterator->first, iterator->second);
			}
		}

		m_categoryBits = 0;
		m_maskBits = 0;
	}

	json TagList::GetData()
	{
		json tagsObjectArray = json::array();
		for (std::map<std::string, bool>::iterator tagIter = m_tags.begin(); tagIter != m_tags.end(); tagIter++)
		{
			// For making sure we don't save any stale tags that aren't available in the Tags.lua file
			for (std::string availableTag : Assets::assetManager.GetTags())
			{
				if (tagIter->first == availableTag)
				{
					json tag = json::object({ { tagIter->first, tagIter->second } });
					tagsObjectArray.push_back(tag);
				}
			}
		}

		json collidesTagsObjectArray = json::array();
		for (std::map<std::string, bool>::iterator tagIter = m_collidesTags.begin(); tagIter != m_collidesTags.end(); tagIter++)
		{
			// For making sure we don't save any stale collides tags that aren't available in the Tags.lua file
			for (std::string availableTag : Assets::assetManager.GetTags())
			{
				if (tagIter->first == availableTag)
				{
					json collides = json::object({ { tagIter->first, tagIter->second } });
					collidesTagsObjectArray.push_back(collides);
				}
			}
		}

		json tagListJson = json::object({
			{ "tags", tagsObjectArray },
			{ "collidesTags", collidesTagsObjectArray }
		});

		return tagListJson;
	}
	
	void TagList::PutData(json taglistJson)
	{
		std::string name = "TagList for GameObject";
		if (JsonHelper::JsonContains(taglistJson, "tags", name))
		{
			json tagsJson = taglistJson["tags"];
			for (json jsonTag : tagsJson)
			{
				std::string tag = jsonTag.items().begin().key();
				bool b_hasTag = jsonTag.items().begin().value();
				SetTag(tag, b_hasTag);
			}
		}
		
		if (JsonHelper::JsonContains(taglistJson, "collidesTags", name))
		{
			json collidesTagsJson = taglistJson["collidesTags"];
			for (json jsonCollidesTag : collidesTagsJson)
			{
				std::string collidesTag = jsonCollidesTag.items().begin().key();
				bool b_collidesTag = jsonCollidesTag.items().begin().value();
				SetCollides(collidesTag, b_collidesTag);
			}
		}			
	}

	void TagList::SetOwnerID(long ownerID)
	{
		m_ownerID = ownerID;
	}

	void TagList::UpdateAvailableTags()
	{
		for (std::string tag : Assets::assetManager.GetTags())
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

		GameObject* owner = SceneManager::loadedScene.GetObjectByID(m_ownerID);
		if (owner != nullptr && owner->Get<Body>() != nullptr)
		{
			owner->Get<Body>()->RecreateLiveBody();
		}
	}

	void TagList::ToggleTag(std::string tag)
	{
		if (m_tags.count(tag) > 0)
		{
			m_tags.at(tag) = !m_tags.at(tag);
		}

		GameObject* owner = SceneManager::loadedScene.GetObjectByID(m_ownerID);
		if (owner != nullptr && owner->Get<Body>() != nullptr)
		{
			owner->Get<Body>()->RecreateLiveBody();
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

		GameObject* owner = SceneManager::loadedScene.GetObjectByID(m_ownerID);
		if (owner != nullptr && owner->Get<Body>() != nullptr)
		{
			owner->Get<Body>()->RecreateLiveBody();
		}
	}

	void TagList::ToggleCollides(std::string tag)
	{
		if (m_collidesTags.count(tag) > 0)
		{
			m_collidesTags.at(tag) = !m_collidesTags.at(tag);
		}

		GameObject* owner = SceneManager::loadedScene.GetObjectByID(m_ownerID);
		if (owner != nullptr && owner->Get<Body>() != nullptr)
		{
			owner->Get<Body>()->RecreateLiveBody();
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