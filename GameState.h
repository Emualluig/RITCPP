#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <optional>
#include <mutex>
#include <memory>

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
	class Decimal {
		// TODO: Implement
	public:
		Decimal() {};
		Decimal(const System::Decimal& d) {};
	};
	class DateTime {
		// TODO: Implement
	public:
		DateTime() {};
		DateTime(const System::DateTime& d) {};
	};
	enum class OrderType {
		MARKET = 0,
		LIMIT = 1,
		TENDER = 2,
		ENDOWMENT = 3,
		TAS = 4
	};

	struct OrderAddMessage {
		int32_t ID;
		int32_t Period;
		Decimal Price;
		int32_t Tick;
		std::string Ticker;
		DateTime Timestamp;
		std::string TraderID;
		OrderType Type;
		Decimal Volume;
		Decimal VolumeRemaining;
		Decimal VWAP;
	};
	struct OrderUpdateMessage {
		int32_t ID;
		std::string TraderID;
		Decimal VolumeRemaining;
	};
	struct TickerWeight {
		std::string Ticker;
		Decimal Weight;
	};
	struct AssetUpdateMessage {
		int32_t ID;
		std::string Ticker;
		std::optional<int32_t> StartLeasePeriod;
		std::optional<int32_t> StartLeaseTick;
		std::optional<int32_t> NextLeasePeriod;
		std::optional<int32_t> NextLeaseTick;
		std::optional<int32_t> ContainmentUsage;
		std::vector<TickerWeight> ConvertFrom;
		std::vector<TickerWeight> ConvertTo;
		std::optional<int32_t> ConvertFinishTick;
		Decimal Realized;
		std::string TraderID;
	};
	struct ContainmentUpdateMessage {
		int32_t ID;
		int32_t Containment;
	};
	struct PortfolioUpdateMessage {
		std::string Ticker;
		Decimal Position;
		Decimal VWAP;
		Decimal Realized;
		std::vector<ContainmentUpdateMessage> UpdatedContainments;
		Decimal PendingPosition;
		Decimal Volume;
	};
	struct TradingLimitUpdateMessage {
		std::string Name;
		Decimal Gross;
		Decimal Net;
	};
	enum class TransactionType {
		ORDER = 0,
		TENDER,
		ENDOWMENT,
		LEASE,
		ASSET_USAGE,
		COMMISSION,
		MARK_TO_MARKET,
		CLOSE_OUT,
		SETTLEMENT,
		DISTRESSED_LEASE,
		DISTRESSED_COVER,
		GROSS_LIMIT_FINE,
		NET_LIMIT_FINE,
		OTC,
		POWER_PLANT,
		LIMIT_REBATE,
		ROWORDER,
		ACCRUED_INTEREST,
		COUPON,
		INTEREST,
		ADJUSTMENT,
		DIVIDEND,
		BOM_FIXING,
		OTC_BREAK,
		DISTRESSED_COVER_FINE,
		DISTRESSED_LEASE_FINE,
		CASE_FINE,
		TYPE_1,
		TYPE_2,
		TYPE_3,
		TYPE_4,
		TYPE_5,
		TYPE_6,
		TYPE_7,
		TYPE_8,
		TYPE_9
	};
	struct TransactionAddMessage {
		int32_t ID;
		std::string Ticker;
		std::optional<Decimal> Price;
		std::optional<Decimal> Quantity;
		TransactionType Type;
		Decimal Value;
		std::string Currency;
		Decimal Balance;
		int32_t Period;
		int32_t Tick;
		std::string TraderID;
		int32_t TypeInteger;
	};
	struct TimeAndSalesUpdateMessage {
		int64_t ID;
		int32_t Period;
		int32_t Tick;
		std::string Ticker;
		Decimal Price;
		Decimal Volume;
		std::string Buyer;
		std::string Seller;
	};
	enum class OTCStatus {
		PENDING = 0,
		ACCEPTED,
		DECLINED,
		CANCELLED,
		BREAK,
		BROKEN
	};
	struct OTCUpdateMessage {
		int32_t ID;
		std::string TraderID;
		std::string Target;
		std::string Ticker;
		Decimal Volume;
		// Depreciated
		Decimal PriceOld;
		OTCStatus Status;
		std::optional<int32_t> SettlePeriod;
		std::optional<int32_t> SettleTick;
		std::string BreakingTraderID;
		std::optional<Decimal> Price;
		std::string PriceExpression;
		std::optional<bool> IsTransacted;
	};
	enum class AssetActionType {
		LEASE = 0,
		END_LEASE = 1,
		RENEW_LEASE = 2,
		LEASE_AND_USE = 3,
		USE = 4,
		USE_COMPLETE = 5
	};
	struct AssetLogUpdateMessage {
		int32_t ID;
		int32_t Period;
		int32_t Tick;
		std::string Ticker;
		AssetActionType Type;
		std::optional<Decimal> Price;
		std::vector<TickerWeight> ConvertFrom;
		std::vector<TickerWeight> ConvertTo;
		std::vector<TickerWeight> ConvertFromPrice;
		std::vector<TickerWeight> ConvertToPrice;
		DateTime Timestamp;
		std::string TraderID;
		std::string Description;
	};

	struct UpdateState {
		int32_t ID;
		int32_t Period;
		int32_t Tick;
		std::vector<OrderAddMessage>* AddedOrders;
		std::vector<OrderUpdateMessage>* UpdatedOrders;
		std::vector<int32_t>* CancelledOrders;
		std::vector<AssetUpdateMessage>* UpdatedAssets;
		std::vector<int32_t>* UnleasedAssets;
		std::map<std::string, Decimal>* UpdatedLast;
		std::map<std::string, Decimal>* UpdatedVolume;
		std::map<std::string, Decimal>* UpdatedVWAP;
		std::map<std::string, PortfolioUpdateMessage>* UpdatedPortfolio;
		std::map<std::string, TradingLimitUpdateMessage>* UpdatedTradingLimits;
		std::vector<TransactionAddMessage>* AddedTransactions;
		std::vector<TimeAndSalesUpdateMessage>* AddedTimeAndSales;
		std::optional<Decimal> UpdatedNLV;
		std::map<std::string, Decimal>* UpdatedSecurityNLV;
		std::vector<std::pair<int32_t, double>>* UpdatedElectricity;
		std::vector<OTCUpdateMessage>* UpdatedOTC;
		std::map<std::string, Decimal>* UpdatedSecurityUnrealized;
		std::vector<AssetLogUpdateMessage>* AddedAssetLogItems;
		~UpdateState() {
			delete this->AddedOrders;
			delete this->UpdatedOrders;
			delete this->CancelledOrders;
			delete this->UpdatedAssets;
			delete this->UnleasedAssets;
			delete this->UpdatedLast;
			delete this->UpdatedVolume;
			delete this->UpdatedVWAP;
			delete this->UpdatedPortfolio;
			delete this->UpdatedTradingLimits;
			delete this->AddedTransactions;
			delete this->AddedTimeAndSales;
			delete this->UpdatedOTC;
			delete this->UpdatedSecurityUnrealized;
			delete this->AddedAssetLogItems;
		}
	};


	class StateManager {
	private:

	public:
		// This is synchronized, only one call at a time
		void HandleOrderedUpdate(std::unique_ptr<Game::UpdateState> state) {}	
	};

	class GameState {
		std::mutex updateMutex = std::mutex();
	public:
		class UpdateGuard {
		private:
			std::mutex* parentMutex;
			explicit UpdateGuard(std::mutex* parentMutex) : parentMutex{ parentMutex } {
				this->parentMutex->lock();
			}
		public:
			~UpdateGuard() {
				this->parentMutex->unlock();
			}
			friend class GameState;
		};
		UpdateGuard lock() {
			return UpdateGuard(&this->updateMutex);
		}

		void AddOrders(const Game::OrderAddMessage& message) {}

		void AddTimeAndSales(const Game::TimeAndSalesUpdateMessage& message) {}

		void UpdateSecurityUnrealized(const std::string& ticker, const Decimal amount) {}

		void UpdateSecurityNLV(const std::string& ticker, const Decimal amount) {}

		void UpdateVWAP(const std::string& ticker, const Decimal amount) {}
		
		void UpdateLast(const std::string& ticker, const Decimal amount) {}

		void AddTransaction(const Game::TransactionAddMessage& message) {}

		// This is the new trading limit
		void UpdateTradingLimits(const std::string& key, const Game::TradingLimitUpdateMessage& message) {}

		// This is the new portfolio
		void UpdatePortfolio(const std::string& key, const Game::PortfolioUpdateMessage& message) {}

		void AddAssetLogItems(const Game::AssetLogUpdateMessage& message) {}

		void UnleaseAsset(int32_t ID) {}

		void UpdateAsset(const Game::AssetUpdateMessage& message) {}

		void CancelledOrder(int32_t ID) {}

		void UpdatedOrder(const Game::OrderUpdateMessage& message) {}

		void UpdateFinished() {}
	};
};

