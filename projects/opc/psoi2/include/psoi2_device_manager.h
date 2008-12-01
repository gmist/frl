#ifndef psoi2_device_manager_h_
#define psoi2_device_manager_h_
#include "poor_xml/frl_poor_xml_document.h"
#include <boost/noncopyable.hpp>
#include "frl_opc.h"
#include <vector>

using namespace frl;
class Psoi2Device;

class DeviceManager : private boost::noncopyable
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
	const std::vector< Psoi2Device* >& getDevices();
	const Psoi2Device* getDevice( frl::UInt port_number );
};

#endif // psoi2_device_manager_h_
