#ifndef psoi2_device_manager_h_
#define psoi2_device_manager_h_
#include "poor_xml/frl_poor_xml_document.h"
#include "frl_non_copyable.h"
#include "frl_opc.h"
#include <vector>

using namespace frl;
class Psoi2Device;

class DeviceManager : private NonCopyable
{
private:
	void initializeAddressSpace();
	void initializeDAServer();
	opc::DAServer *server;
	poor_xml::Document config;
	std::vector< Psoi2Device* > devices;
public:
	DeviceManager();
	~DeviceManager();
};

#endif // psoi2_device_manager_h_
