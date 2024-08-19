#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <map>

namespace Game {
	enum class GameStatus {
		STOPPED = 0,
		ACTIVE,
		PAUSED
	};
	struct GameParameters {
		int32_t Period;
		int32_t Tick;
		int32_t SafeTick;
		int32_t MillisecondsPerTick;
		bool IsActive;
		bool IsPaused;
		GameStatus Status;
	};

	struct OrderAddMessage {};
	struct OrderUpdateMessage {};
	struct AssetUpdateMessage {};

	struct UpdateState {
		int32_t ID;
		int32_t Period;
		int32_t Tick;
		std::vector<OrderAddMessage> AddedOrders;
		std::vector<OrderUpdateMessage> UpdatedOrders;
		std::vector<int32_t> CancelledOrders;
		std::vector<AssetUpdateMessage> UpdatedAssets;
		std::vector<int32_t> UnleasedAssets;
		std::map<std::string, std::string> UpdatedLast;
	};
};

