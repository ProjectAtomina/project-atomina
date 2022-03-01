#pragma once

#include <string>
#include <functional>
#include <memory>
#include <execution>
#include <vector>
#include <set>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <mutex>
#include <random>
#include <bitset>
#include <thread>
#include <future>
#include <chrono>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Main.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <SFML/Window/Event.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

#ifdef _WINDOWS
	#include <Windows.h>
	#include <Shlwapi.h>
	#undef min
	#undef max
#endif