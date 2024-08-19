#include "Client.h"
#include "Secret.h"
#include <cstdint>
#include <limits>
#include <iostream>
#include <thread>

static void OnOpening(System::Object^ sender, System::EventArgs^ e)
{
    std::cout << "OnOpening\n";
}

static void OnOpened(System::Object^ sender, System::EventArgs^ e)
{
    std::cout << "OnOpened\n";
}

static void OnFaulted(System::Object^ sender, System::EventArgs^ e)
{
    std::cout << "OnFaulted\n";
}

static void OnClosing(System::Object^ sender, System::EventArgs^ e)
{
    std::cout << "OnClosing\n";
}

static void OnClosed(System::Object^ sender, System::EventArgs^ e)
{
    std::cout << "OnClosed\n";
}

System::String^ GenerateMD5Hash(System::String^% str) {
    auto bytes = System::Text::Encoding::Unicode->GetBytes(str);
    auto algorithm = System::Security::Cryptography::MD5::Create();
    auto hash = algorithm->ComputeHash(bytes);
    auto sb = gcnew System::Text::StringBuilder(hash->Length);
    for (int i = 0; i < hash->Length; i++) {
        sb->Append(hash[i].ToString("X2"));
    }
    auto retval = sb->ToString();
    delete sb;
    return retval;
}

System::ServiceModel::NetTcpBinding^ CreateTCPBinding() {
    auto binding = gcnew System::ServiceModel::NetTcpBinding(System::ServiceModel::SecurityMode::None);
    binding->MaxReceivedMessageSize = std::numeric_limits<int32_t>::max();
    binding->MaxBufferSize = std::numeric_limits<int32_t>::max();
    binding->MaxBufferPoolSize = std::numeric_limits<int32_t>::max();
    binding->ReaderQuotas->MaxArrayLength = std::numeric_limits<int32_t>::max();
    binding->ReaderQuotas->MaxBytesPerRead = std::numeric_limits<int32_t>::max();
    binding->ReaderQuotas->MaxDepth = std::numeric_limits<int32_t>::max();
    binding->ReaderQuotas->MaxNameTableCharCount = std::numeric_limits<int32_t>::max();
    binding->ReaderQuotas->MaxStringContentLength = std::numeric_limits<int32_t>::max();
    binding->ReliableSession->Enabled = false;
    binding->ReliableSession->Ordered = false;
    binding->ReceiveTimeout = System::TimeSpan(0, 8, 0, 2);
    binding->SendTimeout = System::TimeSpan(0, 0, 0, 22);
    binding->OpenTimeout = System::TimeSpan(0, 0, 0, 12);
    binding->CloseTimeout = System::TimeSpan(0, 0, 0, 8);
    return binding;
}

int main(array<System::String ^> ^args)
{
    System::String^ traderId = gcnew System::String(Secret::traderId);
    System::String^ password = gcnew System::String(Secret::password);
    System::String^ hash = GenerateMD5Hash(password);
    System::String^ hostname = gcnew System::String(Secret::hostname);
    System::String^ port = gcnew System::String(Secret::port);

    auto state = Game::StateManager();
    auto client = gcnew Client(&state);

    System::String^ endpoint = "net.tcp://" + hostname + ":" + port + "/TTSClientService";
    auto binding = CreateTCPBinding();
    auto address = gcnew System::ServiceModel::EndpointAddress(endpoint);
    auto dcf = gcnew System::ServiceModel::DuplexChannelFactory<TTS::IClientService^>(client, binding, address);

    dcf->Opening += gcnew System::EventHandler(&OnOpening);
    dcf->Opened += gcnew System::EventHandler(&OnOpened);
    dcf->Faulted += gcnew System::EventHandler(&OnFaulted);
    dcf->Closing += gcnew System::EventHandler(&OnClosing);
    dcf->Closed += gcnew System::EventHandler(&OnClosed);

    auto channel = dcf->CreateChannel();
    client->SetChannel(channel);
    ((System::ServiceModel::IClientChannel^)channel)->OperationTimeout = System::TimeSpan(0,0,1,0,2);

    bool authenticated = false;
    while (true) {
        if (dcf->State == System::ServiceModel::CommunicationState::Opened && !authenticated) {
            authenticated = true;
            channel->Authenticate(traderId, hash);
        }
        std::this_thread::yield();
    }
    return 0;
}
