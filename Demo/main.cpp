#include <iostream>
#include <ProtocolService.hpp>
using namespace std;

int main(int argc, char** argv) {
    
    cout << "Started demo protocol service" << endl;
    
    std::unique_ptr<StreamSim::Net::ProtocolService> service;

    if (argv[1] == 0) {
        cout << "Running Pooled Service" << endl;
        service = std::make_unique<StreamSim::Net::DemoProtocolServicePooled>(4, 6);
    } else {
        cout << "Running Queued Service" << endl;
        service = std::make_unique<StreamSim::Net::DemoProtocolServiceQueued>(4, 6);
    }
    
    service->Run();
    service->Shutdown();
    cout << "Ended demo protocol service" << endl;

    return 0;
}
