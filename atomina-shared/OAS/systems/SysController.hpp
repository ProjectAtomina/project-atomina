#pragma once
#include "../../pch.hpp"
#include "../../core/api.hpp"
#include "../SysBase.hpp"
#include "../attributes/AttrTranslatable.hpp"
#include "../../event/EventContext.hpp"
#include "../System.hpp"

namespace ATMA
{

	class ATMA_API SysController: public SysBase
	{
	public:
		SysController();

		virtual void update(const float &l_dt) override;

		virtual void move(const EventContext& l_context);


	};

}