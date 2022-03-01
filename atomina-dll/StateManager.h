#pragma once

#include "pch.h"
#include "api.h"
#include "State.h"
#include "BaseState.h"


namespace ATMA
{
	//must be shared because update is parallelized
	using StateContainer = std::vector<std::shared_ptr<BaseState>>;
	
	/*
	* Manager for updating, drawing, and switching states
	*/
	class ATMA_API StateManager
	{

	public:
		StateManager();
		~StateManager();

		/*
		* updates all active states
		* @param time since last update
		*/
		void update(const sf::Time& l_time);

		/*
		* calls draw for each active an non-transparent state
		*/
		void draw();

		/*
		* checks for state in manager
		* @param State type enum
		* @return if it is found or not
		*/
		bool hasState(const State& l_type) const;

		/*
		* get second highest priority state
		* @return type of state
		*/
		State getNextToLast() const;

		/*
		* moves state to front
		* @param type of state
		*/
		void changeState(const State& l_type);

		/*
		* removes the specified state type from the manager
		* @param type of state
		*/
		void remove(const State& l_type);

		/*
		* returns the state object of a given state type
		* @param type of the state
		* @return state object
		*/
		template <class T>
		std::shared_ptr<T> getState(const State& l_type)
		{
			auto result = std::find_if(m_states.begin(), m_states.end(), [&l_type](auto state)
				{
					return state->getId() == l_type;
				}
			);
			return result;
		}

		/*
		* registers the state to the manager
		* @param type of state to register
		*/
		template <class T>
		void registerState(const State &l_type)
		{
			if constexpr (std::is_base_of_v<BaseState, T>)
			{
				auto result = std::find_if(m_states.begin(), m_states.end(), [&l_type](auto state)
					{
						return state->getId() == l_type;
					}
				);
				if(result == m_states.end())
				{
					m_states.emplace_back(std::shared_ptr<T>{new T{}});
					if(auto state = m_states.back(); state->getId() != l_type)
					{
						m_states.pop_back();
					}
				}

			}

		}


		/*
		* removes all states from the manager
		*/
		void purge();


	private:

		StateContainer m_states;



	};

}
