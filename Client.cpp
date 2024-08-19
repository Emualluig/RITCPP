#include "Client.h"
#include <source_location>
#include <iostream>
#include <chrono>

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
	if (!sentSync) {
		return;
	}
}

void Client::OrderedUpdate(TTS::UpdateState^ state)
{
	const std::source_location& location = std::source_location::current();
	std::cout << location.function_name() << "\n";
	std::cout << "\tID:" << state->ID << "\n";

	auto a = std::chrono::high_resolution_clock::now();
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


