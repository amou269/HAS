#ifndef EVALVID_CLIENT_SERVER_HELPER_H
#define EVALVID_CLIENT_SERVER_HELPER_H

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/evalvid-server.h"
#include "ns3/evalvid-client.h"

namespace ns3 {

	class EvalvidServerHelper
	{
		public:
			EvalvidServerHelper ();
			EvalvidServerHelper (uint16_t port);
			void SetAttribute (std::string name, const AttributeValue &value);
			ApplicationContainer Install (NodeContainer c);
			Ptr<EvalvidServer> GetServer(void);
		private:
			ObjectFactory m_factory;
			Ptr<EvalvidServer> m_server;
	};

	class EvalvidClientHelper
	{

		public:
			EvalvidClientHelper ();
			EvalvidClientHelper (Ipv4Address ip,uint16_t port);
			void SetAttribute (std::string name, const AttributeValue &value);
			ApplicationContainer Install (NodeContainer c);

		private:
			ObjectFactory m_factory;
	};

}

#endif
