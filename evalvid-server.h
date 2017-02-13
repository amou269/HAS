#ifndef __EVALVID_SERVER_H__
#define __EVALVID_SERVER_H__

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/ipv4-address.h"
#include "ns3/seq-ts-header.h"
#include "ns3/socket.h"
#include "ns3/traced-callback.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

using namespace std;

namespace ns3 {
	class EvalvidServer : public Application
	{
		public:
			static TypeId GetTypeId (void);
			EvalvidServer ();
			virtual ~EvalvidServer ();

		protected:
			virtual void DoDispose (void);
			uint16_t    m_packetPayload;

		private:
			virtual void StartApplication (void);
			virtual void StopApplication (void);
			void Setup (void);
			void HandleRead (Ptr<Socket> socket);
			void Send();
			void HandleAccept(Ptr<Socket> s, const Address& from);

			string      m_videoTraceFileName;
			string      m_senderTraceFileName;
			fstream     m_videoTraceFile;
			ofstream    m_senderTraceFile;
			uint32_t    m_numOfFrames;
			uint32_t    m_packetId;
			uint16_t    m_port;
			Ptr<Socket> m_socket;
			Address     m_peerAddress;
			EventId     m_sendEvent;

			struct m_videoInfoStruct_t
			{
				string   frameType;
				uint32_t frameSize;
				uint16_t numOfTcpPackets;
				Time     packetInterval;
			};

			map<uint32_t, m_videoInfoStruct_t*> m_videoInfoMap;
			map<uint32_t, m_videoInfoStruct_t*>::iterator m_videoInfoMapIt;
			uint32_t m_totalRx;
			TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;

		/*	Client request info*/
        
			uint32_t clientResolution;
			uint32_t segmentId;
			uint32_t videoId;
			int packetcount;
	};
}
#endif
