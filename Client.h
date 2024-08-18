#pragma once

[System::ServiceModel::CallbackBehaviorAttribute(ConcurrencyMode = System::ServiceModel::ConcurrencyMode::Multiple, UseSynchronizationContext = false)]
public ref class Client : TTS::IClientCallbackService {
private:
	TTS::IClientService^ channel = nullptr;
public:
	// Required for bi-directional communication
	void SetChannel(TTS::IClientService^ channel) {
		this->channel = channel;
	}
	TTS::IClientService^ GetChannel() {
		return this->channel;
	}
	// Inherited via IClientCallbackService
	virtual void Ping();
	virtual void SendMessage(System::String^ message, TTS::SystemMessageType type);
	virtual void SendNewTraderMessage(System::String^ traderid);
	virtual void Sync(TTS::SyncState^ state);
	virtual void OrderedUpdate(TTS::UpdateState^ state);
	virtual void Reset(TTS::SyncState^ state);
	virtual void Heartbeat(int tick);
	virtual void UpdateGameStatus(TTS::GameParameters^ current);
	virtual void UpdateAssetLeaseCount(TTS::AssetItemUpdateMessage^ item);
	virtual void UpdateNews(int id, System::String^ ticker, System::String^ headline, System::String^ body);
	virtual void TenderOffer(int id, int expiretick, System::String^ ticker, System::String^ caption, System::Decimal bid, System::Decimal volume);
	virtual void CloseTenderOffer(int id);
	virtual void AddOrderHistory(int id, System::String^ ticker, System::Decimal price, System::Decimal volume, TTS::OrderType type, int period, int tick, TTS::OrderStatus status);
	virtual void UpdateVariable(TTS::VariablesUpdateMessage^ update);
	virtual void AddConnectedTraders(array<System::String^, 1>^ traderids);
	virtual void RemoveConnectedTrader(System::String^ traderid);
	virtual void AddConnectedAssessors(array<System::String^, 1>^ traderids);
	virtual void RemoveConnectedAssessor(System::String^ traderid);
	virtual void ChatMessage(System::String^ from, System::String^ to, System::String^ message);
	virtual void ToggleReportDownload(bool visible);
	virtual void ComplianceMessage(int id, TTS::MessageType type, System::String^ message);
	virtual void CallRequestRejected(System::String^ caller);
	virtual void InitiateVoiceConnection(unsigned int port);
	virtual void PushToTalkTriggered();
	virtual void UpdateVoiceChatState(TTS::VoiceChatState^ state);
	virtual void UpdateAssessorState(TTS::AssessorUpdate^ state);
	virtual void AddAssessorTraderGroupState(TTS::AssessorTraderGroupSyncState^ state);
	virtual void UpdateAssesorNews(array<TTS::NewsHistoryUpdateMessage^, 1>^ rows);
	virtual void UpdateAssessorVoiceChatState(TTS::AssessorVoiceChatState^ state);
	virtual void AddAdHocFile(int index, System::String^ name);
};
