#pragma once


namespace ATMA {

	using SystemType = unsigned int;

	enum class ATMA_API System {
		None = 0u,
		Translator = 1u,
		Renderer,
		Controller,
		COUNT
	};

}