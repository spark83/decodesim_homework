#include <iostream>
#include <ProtocolService.hpp>
using namespace std;

int main() {
    std::unique_ptr<StreamSim::Net::ProtocolService> service = std::make_unique<StreamSim::Net::DemoProtocolService>(4, 6);
    
    cout << "Started demo protocol service" << endl;
    
    service->Run();
    service->Shutdown();
    
    cout << "Ended demo protocol service" << endl;

    return 0;
}
