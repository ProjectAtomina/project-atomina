#pragma once
#include "../pch.hpp"
#include "../util/ATConst.hpp"
#include "Attribute.hpp"
#include "AttrBase.hpp"
#include "../util/AtominaException.hpp"
#include "../core/api.hpp"
#include "../util/Log.hpp"


namespace ATMA {

	using ObjectId = unsigned int;

	class ATMA_API ObjectManager {

		using ObjectAttributes = std::pair<std::bitset<ATConst::OBJECT_BIT_SIZE>, std::unordered_map<AttributeType,std::shared_ptr<AttrBase>>>;
	public:

		//TODO: add Texture Manager 
		ObjectManager();
		~ObjectManager();

		/**
		* Increments the id of the objectmanager and creates attributes for each active bit in the given bitset
		* assigns attributes to the id in the object map and returns the new id
		*/
		ObjectId createObject(const std::bitset<ATConst::OBJECT_BIT_SIZE>&);

		/**
		* Increments the id of the objectmanager and returns the id
		* note that since no attributes were created the id does not yet exist in the object map
		*/
		ObjectId createObject();

		/**
		* removes the object and all attributes from the object map
		* only returns true if successful
		*/
		bool removeObject(const ObjectId& l_id);

		/**
		* Creates an attribute using the attribute factory and assigns it to given object id
		* returns true only if successful
		*/
		bool addAttribute(const ObjectId &l_id, const Attribute &l_attr);

		/**
		* Creates an attribute using the attribute factory and assigns it to given object id
		* returns true only if successful
		*/
		bool addAttribute(const ObjectId& l_id, const AttributeType &l_attr);

		/**
		* Creates attribute factory for the given attribute enum and type associaton
		* note association persists in the attribute factory
		*/
		template<class T>
		void addAttributeType(const Attribute& l_attr) {
			addAttributeType<T>(static_cast<AttributeType>(l_attr));
		}

		/**
		* Creates attribute factory for the given attribute enum and type associaton
		* note association persists in the attribute factory
		*/
		template<class T>
		void addAttributeType(const AttributeType &l_attr)
		{
			if constexpr(std::is_base_of_v<AttrBase, T>)
			{
			//lambda to add factory function, allows correspondence of template and enum
				ATMA_ENGINE_INFO("Adding {0} to object manager", l_attr);
				m_attrFactory[l_attr] = []()->std::shared_ptr<AttrBase> { return std::shared_ptr<T>{new T()}; };
			}
			else
			{
				throw std::bad_cast{};
			}
		}

		/**
		* Gets the pointer to attribute of the given object and enum association
		* throws an ObjectNotFoundException when the object or attribute does not exist in the objects map
		*/
		template<class T>
		std::shared_ptr<T> getAttribute(const ObjectId &l_id, const Attribute &l_attr)
		{
			if constexpr(std::is_base_of_v<AttrBase, T>)
			{
				auto attr = static_cast<AttributeType>(l_attr);
				if(m_objects.find(l_id) == m_objects.end())
				{
					throw ValueNotFoundException("object id: " + std::to_string(l_id) + " does not exist");
				}
				else if(!m_objects[l_id].first[attr])
				{
					throw ValueNotFoundException("object id: " + std::to_string(l_id) + " does not contain attribute: " + std::to_string(static_cast<unsigned int>(l_attr)));
				}

				return std::static_pointer_cast<T>(m_objects[l_id].second[attr]);
			}
			else
			{
				throw std::bad_cast{};
			}
		}

		/**
		* Gets the pointer to attribute of the given object and enum association
		* throws an ObjectNotFoundException when the object or attribute does not exist in the objects map
		*/
		template<class T>
		std::shared_ptr<T> getAttribute(const ObjectId& l_id, const AttributeType &l_attr) {
			if constexpr(std::is_base_of_v<AttrBase, T>)
			{
				if(m_objects.find(l_id) == m_objects.end())
				{
					throw ValueNotFoundException("object id: " + std::to_string(l_id) + " does not exist");
				}
				else if(!m_objects[l_id].first[l_attr])
				{
					throw ValueNotFoundException("object id: " + std::to_string(l_id) + " does not contain attribute: " + std::to_string(l_attr));
				}

				return std::static_pointer_cast<T>(m_objects[l_id].second[l_attr]);
			}
			else
			{
				throw std::bad_cast("unable to cast as type is not subclass of AttrBase");
			}

		}


		/**
		* removes and deletes an attribute from the object id
		* returns true only if succesful
		*/
		bool removeAttribute(const ObjectId &l_id, const Attribute &l_attr);

		/**
		* removes and deletes an attribute from the object id
		* returns true only if succesful
		*/
		bool removeAttribute(const ObjectId& l_id, const AttributeType &l_attr);
		
		/**
		* checks if the given object id has the given attribute
		*/
		bool hasAttribute(const ObjectId& l_id, const Attribute &l_attr);

				/**
		* checks if the given object id has the given attribute
		*/
		bool hasAttribute(const ObjectId &l_id, const AttributeType &l_attr);

		/**
		* Clears all objects, clears all factory functions, and sets lastId to 0
		*/
		void purge();

	private:
		
		std::mutex m_mtx{};
		ObjectId m_lastId{0u};
		std::unordered_map<ObjectId, ObjectAttributes> m_objects{};
		std::unordered_map<AttributeType, std::function<std::shared_ptr<AttrBase>(void)>> m_attrFactory{};
		

	};
}