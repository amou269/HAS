#ifndef __EVALVID_CLIENT_H__
#define __EVALVID_CLIENT_H__

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/seq-ts-header.h"
#include "ns3/traced-callback.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "http-parser.h"
#include "mpeg-player.h"

using namespace std;

namespace ns3 {

class Socket;
class Packet;

	class EvalvidClient : public Application
	{
		friend class MpegPlayer;
		friend class HttpParser;

		public:
			static TypeId GetTypeId (void);
			EvalvidClient ();
			virtual ~EvalvidClient ();
			void SetRemote (Ipv4Address ip, uint16_t port);
			MpegPlayer& GetPlayer();

		protected:
			virtual void DoDispose (void);
			double GetBitRateEstimate();

			uint32_t m_rateChanges;
			map<Time, Time> m_bufferState;
			map<Time, double> m_bitrates;
			double m_bitrateEstimate;
			uint16_t    m_packetPayload;

		private:
			virtual void StartApplication (void);
			virtual void StopApplication (void);

			void ConnectionSucceeded (Ptr<Socket> socket);
			void ConnectionFailed (Ptr<Socket> socket);

			void Send (void);
			void HandleRead (Ptr<Socket> socket);

			void MessageReceived(Packet message);
			virtual void CalcNextSegment(uint32_t currRate, uint32_t & nextRate, Time & delay);
			void LogBufferLevel(Time t);
			void AddBitRate(Time time, double bitrate);

			ofstream    receiverDumpFile;
			string      receiverDumpFileName;
			Ptr<Socket> m_socket;
			Ipv4Address m_peerAddress;
			uint16_t    m_peerPort;
			EventId     m_sendEvent;
			uint32_t	m_totBytes;		// Total bytes received.
			uint32_t 	m_segment_bytes;	// Bytes of the current segment that have been received so far.
			Time 		m_segmentFetchTime;
			ns3::Time 	m_requestTime;
			Time 		m_window;
			MpegPlayer 	m_player;
			HttpParser 	m_parser;

		/*	Client request info*/

			uint32_t 	m_bitRate;			// Bitrate of current segment resolution
			uint32_t 	m_segmentId;
			uint32_t 	m_videoId;
			int 		packetcount;

			Time 		m_sumDt;	// Used for calculating the average buffering time.
			Time 		m_lastDt;	// The previous buffering time (used for calculating the differential.

			int m_id; // The node id.
	};
}
#endif
