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
	constexpr const static int32_t DEFAULT_ALLOC_SIZE = 4;
	auto us = std::unique_ptr<Game::UpdateState>(new Game::UpdateState{
		.ID = update->ID,
		.Period = update->Period,
		.Tick = update->Tick,
		.AddedOrders = nullptr,
		.UpdatedOrders = nullptr,
		.CancelledOrders = nullptr,
		.UpdatedAssets = nullptr,
		.UnleasedAssets = nullptr,
		.UpdatedLast = nullptr,
		.UpdatedVolume = nullptr,
		.UpdatedVWAP = nullptr,
		.UpdatedPortfolio = nullptr,
		.UpdatedTradingLimits = nullptr,
		.AddedTransactions = nullptr,
		.AddedTimeAndSales = nullptr,
		.UpdatedNLV = unwrapDecimal(update->UpdatedNLV),
		.UpdatedSecurityNLV = nullptr,
		.UpdatedElectricity = nullptr,
		.UpdatedOTC = nullptr,
		.UpdatedSecurityUnrealized = nullptr,
		.AddedAssetLogItems = nullptr,
		});
	if (update->AddedOrders != nullptr) {
		auto vec = new std::vector<Game::OrderAddMessage>(DEFAULT_ALLOC_SIZE);
		for each (TTS::OrderAddMessage^% var in update->AddedOrders)
		{
			vec->push_back(Game::OrderAddMessage{
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
		us->AddedOrders = vec;
	}
	if (update->UpdatedOrders != nullptr) {
		auto vec = new std::vector<Game::OrderUpdateMessage>(DEFAULT_ALLOC_SIZE);
		for each (TTS::OrderUpdateMessage^% var in update->UpdatedOrders)
		{
			vec->push_back({
				.ID = var->ID,
				.TraderID = msclr::interop::marshal_as<std::string>(var->TraderID),
				.VolumeRemaining = var->VolumeRemaining,
			});
		}
		us->UpdatedOrders = vec;
	}
	if (update->CancelledOrders != nullptr) {
		auto vec = new std::vector<int32_t>(DEFAULT_ALLOC_SIZE);
		for each (auto var in update->CancelledOrders)
		{
			vec->push_back(var);
		}
		us->CancelledOrders = vec;
	}
	if (update->UpdatedAssets != nullptr) {
		auto vec = new std::vector<Game::AssetUpdateMessage>(DEFAULT_ALLOC_SIZE);
		for each (TTS::AssetUpdateMessage^% var in update->UpdatedAssets)
		{
			vec->push_back({
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
		us->UpdatedAssets = vec;
	}
	if (update->UnleasedAssets != nullptr) {
		auto vec = new std::vector<int32_t>(DEFAULT_ALLOC_SIZE);
		for each (auto var in update->UnleasedAssets)
		{
			vec->push_back(var);
		}
		us->UnleasedAssets = vec;
	}
	if (update->AddedAssetLogItems != nullptr) {
		auto vec = new std::vector<Game::AssetLogUpdateMessage>(DEFAULT_ALLOC_SIZE);
		for each (TTS::AssetLogUpdateMessage^% var in update->AddedAssetLogItems)
		{
			vec->push_back({
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
		us->AddedAssetLogItems = vec;
	}
	if (update->UpdatedPortfolio != nullptr) {
		auto map = new std::map<std::string, Game::PortfolioUpdateMessage>();
		for each (auto% var in update->UpdatedPortfolio)
		{
			auto% value = var.Value;
			map->insert({
				msclr::interop::marshal_as<std::string>(var.Key),
				{
					.Ticker = msclr::interop::marshal_as<std::string>(value->Ticker),
					.Position = value->Position,
					.VWAP = value->VWAP,
					.Realized = value->Realized,
					.UpdatedContainments = getContainmentUpdateMessage(value->UpdatedContainments),
					.PendingPosition = value->PendingPosition,
					.Volume = value->Volume,
				}
			});
		}
		us->UpdatedPortfolio = map;
	}
	if (update->UpdatedTradingLimits != nullptr) {
		auto map = new std::map<std::string, Game::TradingLimitUpdateMessage>();
		for each (auto% var in update->UpdatedTradingLimits)
		{
			auto% value = var.Value;
			map->insert({
				msclr::interop::marshal_as<std::string>(var.Key),
				{
					.Name = msclr::interop::marshal_as<std::string>(value->Name),
					.Gross = value->Gross,
					.Net = value->Net,
				}
			});
		}
		us->UpdatedTradingLimits = map;
	}
	if (update->AddedTransactions != nullptr) {
		auto vec = new std::vector<Game::TransactionAddMessage>();
		for each (auto% var in update->AddedTransactions)
		{
			vec->push_back({
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
		us->AddedTransactions = vec;
	}
	if (update->UpdatedLast != nullptr) {
		auto map = new std::map<std::string, Game::Decimal>();
		for each (auto% var in update->UpdatedLast)
		{
			map->insert({
				msclr::interop::marshal_as<std::string>(var.Key), var.Value
			});
		}
		us->UpdatedLast = map;
	}
	if (update->UpdatedVWAP != nullptr) {
		auto map = new std::map<std::string, Game::Decimal>();
		for each (auto% var in update->UpdatedVWAP)
		{
			map->insert({
				msclr::interop::marshal_as<std::string>(var.Key), var.Value
			});
		}
		us->UpdatedVWAP = map;
	}
	if (update->UpdatedSecurityNLV != nullptr) {
		auto map = new std::map<std::string, Game::Decimal>();
		for each (auto % var in update->UpdatedSecurityNLV)
		{
			map->insert({
				msclr::interop::marshal_as<std::string>(var.Key), var.Value
			});
		}
		us->UpdatedSecurityNLV = map;
	}
	if (update->UpdatedSecurityUnrealized != nullptr) {
		auto map = new std::map<std::string, Game::Decimal>();
		for each (auto % var in update->UpdatedSecurityUnrealized)
		{
			map->insert({
				msclr::interop::marshal_as<std::string>(var.Key), var.Value
			});
		}
		us->UpdatedSecurityUnrealized = map;
	}
	if (update->AddedTimeAndSales != nullptr) {
		auto vec = new std::vector<Game::TimeAndSalesUpdateMessage>();
		for each (auto% var in update->AddedTimeAndSales)
		{
			vec->push_back({
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
		us->AddedTimeAndSales = vec;
	}
	if (update->UpdatedOTC != nullptr) {
		// NOTE: This isn't used on the API
		auto vec = new std::vector<Game::OTCUpdateMessage>();
		for each (auto% var in update->UpdatedOTC)
		{
			vec->push_back({
				.ID = var->ID,
				.TraderID = msclr::interop::marshal_as<std::string>(var->TraderID),
				.Target = msclr::interop::marshal_as<std::string>(var->Target),
				.Ticker = msclr::interop::marshal_as<std::string>(var->Ticker),
				.Volume = var->Volume,
				.PriceOld = var->PriceOld,
				.Status = (Game::OTCStatus)(int32_t)var->Status,
				.SettlePeriod = unwrapInt(var->SettlePeriod),
				.SettleTick = unwrapInt(var->SettleTick),
				.BreakingTraderID = msclr::interop::marshal_as<std::string>(var->BreakingTraderID),
				.Price = unwrapDecimal(var->Price),
				.PriceExpression = msclr::interop::marshal_as<std::string>(var->PriceExpression),
				.IsTransacted = unwrapBool(var->IsTransacted)
			});
		}
		us->UpdatedOTC = vec;
	}
	if (update->UpdatedElectricity != nullptr) {
		// NOTE: This isn't used by API
		auto vec = new std::vector<std::pair<int32_t, double>>();
		for each (auto% var in update->UpdatedElectricity)
		{
			vec->push_back({
				var->Item1, var->Item2	
			});
		}
		us->UpdatedElectricity = vec;
	}
	this->StateManager->HandleOrderedUpdate(std::move(us));
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


