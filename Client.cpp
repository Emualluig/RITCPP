#include "Client.h"
#include "GameState.h"
#include <source_location>
#include <iostream>
#include <chrono>
#include <optional>
#include <memory>
#include <msclr\marshal_cppstd.h>

void Client::Ping()
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::SendMessage(System::String^ message, TTS::SystemMessageType type)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::SendNewTraderMessage(System::String^ traderid)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::Sync(TTS::SyncState^ state)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
	bool sentSync = false;
	if (this->Channel != nullptr) {
		try {
			Channel->SyncReceived();
			sentSync = true;
		}
		catch (System::Exception^ e) {
			std::cout << "\tReceived Managed exception when calling channel->SyncReceived()\n";
			System::Console::WriteLine(e);
		}
		catch (std::exception& e) {
			std::cout << "\tReceived C++ exception when calling channel->SyncReceived()\n";
			std::cout << "\t" << e.what();
		}
	}
	else {
		// this->channel == nullptr
		std::cout << "\tUnable to get communication channel. this->channel == nullptr\n";
	}
	if (sentSync == false) {
		return;
	}
	this->isSynced = true;
}

static std::optional<int32_t> unwrapInt(System::Nullable<int>% val) {
	if (val.HasValue) {
		return val.Value;
	}
	else {
		return std::nullopt;
	}
}
static std::optional<Game::Decimal> unwrapDecimal(System::Nullable<System::Decimal>% val) {
	if (val.HasValue) {
		return Game::Decimal(val.Value);
	}
	else {
		return std::nullopt;
	}
}
static std::optional<bool> unwrapBool(System::Nullable<bool>% val) {
	if (val.HasValue) {
		return val.Value;
	}
	else {
		return std::nullopt;
	}
}
static std::vector<Game::TickerWeight> getTickerWeight(array<TTS::TickerWeight^>^% tw) {
	std::vector<Game::TickerWeight> retval = {};
	for each (TTS::TickerWeight^% var in tw)
	{
		retval.push_back({
			.Ticker = msclr::interop::marshal_as<std::string>(var->Ticker),
			.Weight = var->Weight,
		});
	}
	return retval;
}
static std::vector<Game::ContainmentUpdateMessage> getContainmentUpdateMessage(array<TTS::ContainmentUpdateMessage^>^% cum) {
	std::vector<Game::ContainmentUpdateMessage> retval = {};
	for each (auto% var in cum)
	{
		retval.push_back({
			.ID = var->ID,	
			.Containment = var->Containment
		});
	}
	return retval;
}

void Client::HandleSynchronizedUpdate(TTS::UpdateState^ update) {
	{
		auto guard = this->GameState->lock();

		if (update->AddedOrders != nullptr) {
			for each (TTS::OrderAddMessage ^ %var in update->AddedOrders)
			{
				this->GameState->AddOrders(Game::OrderAddMessage{
					.ID = var->ID,
					.Period = var->Period,
					.Price = var->Price,
					.Tick = var->Tick,
					.Ticker = msclr::interop::marshal_as<std::string>(var->Ticker),
					.Timestamp = var->Timestamp,
					.TraderID = msclr::interop::marshal_as<std::string>(var->TraderID),
					.Type = (Game::OrderType)(int32_t)var->Type,
					.Volume = var->Volume,
					.VolumeRemaining = var->VolumeRemaining,
					.VWAP = var->VWAP,
					});
			}
		}
		if (update->UpdatedOrders != nullptr) {
			for each (TTS::OrderUpdateMessage ^ %var in update->UpdatedOrders)
			{
				this->GameState->UpdatedOrder(Game::OrderUpdateMessage{
					.ID = var->ID,
					.TraderID = msclr::interop::marshal_as<std::string>(var->TraderID),
					.VolumeRemaining = var->VolumeRemaining,
					});
			}
		}
		if (update->CancelledOrders != nullptr) {
			for each (auto var in update->CancelledOrders)
			{
				this->GameState->CancelledOrder(var);
			}
		}
		if (update->UpdatedAssets != nullptr) {
			for each (TTS::AssetUpdateMessage ^ %var in update->UpdatedAssets)
			{
				this->GameState->UpdateAsset(Game::AssetUpdateMessage{
					.ID = var->ID,
					.Ticker = msclr::interop::marshal_as<std::string>(var->Ticker),
					.StartLeasePeriod = unwrapInt(var->StartLeasePeriod),
					.StartLeaseTick = unwrapInt(var->StartLeaseTick),
					.NextLeasePeriod = unwrapInt(var->NextLeasePeriod),
					.NextLeaseTick = unwrapInt(var->NextLeaseTick),
					.ContainmentUsage = unwrapInt(var->ContainmentUsage),
					.ConvertFrom = getTickerWeight(var->ConvertFrom),
					.ConvertTo = getTickerWeight(var->ConvertTo),
					.ConvertFinishTick = unwrapInt(var->ConvertFinishTick),
					.Realized = var->Realized,
					.TraderID = msclr::interop::marshal_as<std::string>(var->TraderID)
					});
			}
		}
		if (update->UnleasedAssets != nullptr) {
			for each (auto var in update->UnleasedAssets)
			{
				this->GameState->UnleaseAsset(var);
			}
		}
		if (update->AddedAssetLogItems != nullptr) {
			for each (TTS::AssetLogUpdateMessage ^ %var in update->AddedAssetLogItems)
			{
				this->GameState->AddAssetLogItems(Game::AssetLogUpdateMessage{
					.ID = var->ID,
					.Period = var->Period,
					.Tick = var->Tick,
					.Ticker = msclr::interop::marshal_as<std::string>(var->Ticker),
					.Type = (Game::AssetActionType)(int32_t)var->Type,
					.Price = unwrapDecimal(var->Price),
					.ConvertFrom = getTickerWeight(var->ConvertFrom),
					.ConvertTo = getTickerWeight(var->ConvertTo),
					.ConvertFromPrice = getTickerWeight(var->ConvertFromPrice),
					.ConvertToPrice = getTickerWeight(var->ConvertToPrice),
					.Timestamp = var->Timestamp,
					.TraderID = msclr::interop::marshal_as<std::string>(var->TraderID),
					.Description = msclr::interop::marshal_as<std::string>(var->Description)
					});
			}
		}
		if (update->UpdatedPortfolio != nullptr) {
			for each (auto % var in update->UpdatedPortfolio)
			{
				auto% value = var.Value;
				this->GameState->UpdatePortfolio(msclr::interop::marshal_as<std::string>(var.Key), Game::PortfolioUpdateMessage{
					.Ticker = msclr::interop::marshal_as<std::string>(value->Ticker),
					.Position = value->Position,
					.VWAP = value->VWAP,
					.Realized = value->Realized,
					.UpdatedContainments = getContainmentUpdateMessage(value->UpdatedContainments),
					.PendingPosition = value->PendingPosition,
					.Volume = value->Volume,
					});
			}
		}
		if (update->UpdatedTradingLimits != nullptr) {
			for each (auto % var in update->UpdatedTradingLimits)
			{
				auto% value = var.Value;
				this->GameState->UpdateTradingLimits(msclr::interop::marshal_as<std::string>(var.Key), Game::TradingLimitUpdateMessage{
					.Name = msclr::interop::marshal_as<std::string>(value->Name),
					.Gross = value->Gross,
					.Net = value->Net,
					});
			}
		}
		if (update->AddedTransactions != nullptr) {
			for each (auto % var in update->AddedTransactions)
			{
				this->GameState->AddTransaction(Game::TransactionAddMessage{
					.ID = var->ID,
					.Ticker = msclr::interop::marshal_as<std::string>(var->Ticker),
					.Price = unwrapDecimal(var->Price),
					.Quantity = unwrapDecimal(var->Quantity),
					.Type = (Game::TransactionType)(int32_t)var->Type,
					.Value = var->Value,
					.Currency = msclr::interop::marshal_as<std::string>(var->Currency),
					.Balance = var->Balance,
					.Period = var->Period,
					.Tick = var->Tick,
					.TraderID = msclr::interop::marshal_as<std::string>(var->TraderID),
					.TypeInteger = var->TypeInteger,
					});
			}
		}
		if (update->UpdatedLast != nullptr) {
			for each (auto % var in update->UpdatedLast)
			{
				this->GameState->UpdateLast(msclr::interop::marshal_as<std::string>(var.Key), var.Value);
			}
		}
		if (update->UpdatedVWAP != nullptr) {
			for each (auto % var in update->UpdatedVWAP)
			{
				this->GameState->UpdateVWAP(msclr::interop::marshal_as<std::string>(var.Key), var.Value);
			}
		}
		if (update->UpdatedSecurityNLV != nullptr) {
			for each (auto % var in update->UpdatedSecurityNLV)
			{
				this->GameState->UpdateSecurityNLV(msclr::interop::marshal_as<std::string>(var.Key), var.Value);
			}
		}
		if (update->UpdatedSecurityUnrealized != nullptr) {
			for each (auto % var in update->UpdatedSecurityUnrealized)
			{
				this->GameState->UpdateSecurityUnrealized(msclr::interop::marshal_as<std::string>(var.Key), var.Value);
			}
		}
		if (update->AddedTimeAndSales != nullptr) {
			for each (auto % var in update->AddedTimeAndSales)
			{
				this->GameState->AddTimeAndSales(Game::TimeAndSalesUpdateMessage{
					.ID = var->ID,
					.Period = var->Period,
					.Tick = var->Tick,
					.Ticker = msclr::interop::marshal_as<std::string>(var->Ticker),
					.Price = var->Price,
					.Volume = var->Volume,
					.Buyer = msclr::interop::marshal_as<std::string>(var->Buyer),
					.Seller = msclr::interop::marshal_as<std::string>(var->Seller)
					});
			}
		}
		if (update->UpdatedOTC != nullptr) {
			// NOTE: This isn't used on the API
		}
		if (update->UpdatedElectricity != nullptr) {
			// NOTE: This isn't used by API
		}
	}
	this->GameState->UpdateFinished();
}

void Client::SynchronizeOrderedUpdate(TTS::UpdateState^ update) {
	std::lock_guard lock = std::lock_guard(*this->PriorityQueueMutex);
	if (update != nullptr) {
		PriorityQueue->push({
			.ID = update->ID,
			.update = update,
		});
	}
	while (this->isSynced && this->PriorityQueue->empty() == false && this->PriorityQueue->top().ID == ExpectedUpdateID) {
		auto state = this->PriorityQueue->top();
		this->HandleSynchronizedUpdate(state.update);
		this->ExpectedUpdateID += 1;
		this->PriorityQueue->pop();
	}
}

void Client::OrderedUpdate(TTS::UpdateState^ state)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
	std::cout << "\tID:" << state->ID << "\n";

	auto t0 = std::chrono::high_resolution_clock::now();
	// We want to synchronize such that we update according to the state's ID in an ascending order
	// i.e.: 0 then 1
	this->SynchronizeOrderedUpdate(state);
	auto t1 = std::chrono::high_resolution_clock::now();
	std::cout << "\ttook: " << std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0) << "\n";
}

void Client::Reset(TTS::SyncState^ state)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::Heartbeat(int tick)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::UpdateGameStatus(TTS::GameParameters^ current)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::UpdateAssetLeaseCount(TTS::AssetItemUpdateMessage^ item)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::UpdateNews(int id, System::String^ ticker, System::String^ headline, System::String^ body)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::TenderOffer(int id, int expiretick, System::String^ ticker, System::String^ caption, System::Decimal bid, System::Decimal volume)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::CloseTenderOffer(int id)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::AddOrderHistory(int id, System::String^ ticker, System::Decimal price, System::Decimal volume, TTS::OrderType type, int period, int tick, TTS::OrderStatus status)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::UpdateVariable(TTS::VariablesUpdateMessage^ update)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::AddConnectedTraders(array<System::String^, 1>^ traderids)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::RemoveConnectedTrader(System::String^ traderid)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::AddConnectedAssessors(array<System::String^, 1>^ traderids)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::RemoveConnectedAssessor(System::String^ traderid)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::ChatMessage(System::String^ from, System::String^ to, System::String^ message)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::ToggleReportDownload(bool visible)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::ComplianceMessage(int id, TTS::MessageType type, System::String^ message)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::CallRequestRejected(System::String^ caller)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::InitiateVoiceConnection(unsigned int port)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::PushToTalkTriggered()
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::UpdateVoiceChatState(TTS::VoiceChatState^ state)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::UpdateAssessorState(TTS::AssessorUpdate^ state)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::AddAssessorTraderGroupState(TTS::AssessorTraderGroupSyncState^ state)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::UpdateAssesorNews(array<TTS::NewsHistoryUpdateMessage^, 1>^ rows)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::UpdateAssessorVoiceChatState(TTS::AssessorVoiceChatState^ state)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}

void Client::AddAdHocFile(int index, System::String^ name)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
}


