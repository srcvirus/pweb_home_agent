/*
 * dns_message_handler.cpp
 *
 *  Created on: 2014-01-23
 *      Author: sr2chowd
 */

#include "dns_message_handler.h"
#include "../../global.h"
#include "../../controllers/home_agent_index_cassandra_controller.h"
#include "../../models/home_agent_index.h"
#include "../../database/cassandra_db.h"
#include "../../controllers/device_cassandra_controller.h"

#include <boost/array.hpp>

#include <pthread.h>

using namespace boost;
using namespace std;

void DNSMessageHandler::handleDNSQueryRecive(
    boost::array<char, MAX_UDP_BUFFER_SIZE> buffer, size_t bytesReceived,
    boost::shared_ptr<UDPConnection> connection) {
  boost::asio::ip::udp::endpoint remoteEndpoint =
      connection->getRemoteEndpoint();
  unsigned long tid = (unsigned long)pthread_self();
  LOG(DEBUG) << "[Thread " << std::hex << tid << "] "
             << "Processing DNS query";
  int byteOffset = 0;
  DNSMessage dnsQueryMessage(buffer.c_array());
  dnsQueryMessage.parse();
  string ansToQuestion = "";
  bool isQuery = !dnsQueryMessage.getDNSHeader().getQR();
  if (isQuery) {
    bool isValid =
        this->isValidRequest(dnsQueryMessage, connection->getSuffix());
    if (isValid) {
      vector<DNSQuestion> &questions = dnsQueryMessage.getDNSQuestions();
      DNSQuestion question = questions[0];
      vector<string> &labels = question.getLabels();
      string haAlias, user, device;
      if (labels.size() <= 4)
        haAlias = labels[0];
      else
        haAlias = labels[2];
      boost::shared_ptr<CassandraDBDriver> dbDriver =
          boost::dynamic_pointer_cast<CassandraDBDriver>(
              CassandraDBDriver::getDatabaseDriverObject());
      LOG(DEBUG) << "[Thread " << std::hex << tid << "] "
                 << "haAlias = " << haAlias.c_str()
                 << ", user = " << user.c_str()
                 << ", device = " << device.c_str();
      if (haAlias == connection->getAlias()) {
        string ip;

        // This is the target home agent.
        if (labels.size() > 4) {
          string &user = labels[1];
          string &device = labels[0];
          DeviceCassandraController dController(dbDriver);
          ip = dController.getDeviceIp(device, user);
          if (ip.size() <= 0) isValid = false;
        } else {
          HomeAgentIndexCassandraController haIndexController(dbDriver);
          boost::shared_ptr<HomeAgentIndex> haIndex =
              haIndexController.getHomeAgentIndex(haAlias);
          if (haIndex) {
            ip = haIndex->getIp();
          }
        }
        if (isValid) {
          ansToQuestion = ip;
          LOG(DEBUG) << "[Thread " << std::hex << tid << "] "
                     << "Query resolved to " << ip.c_str();
          DNSMessage reply;
          this->composeSuccessReply(dnsQueryMessage, reply, ansToQuestion);
          reply.print();
          this->forwardDNSMessage(reply, remoteEndpoint, connection);
        }
      } else {
        // This is not the target home agent. Lookup home agent's alias in
        //  cassandra to forward to query to the target home agent.
        HomeAgentIndexCassandraController haIndexController(dbDriver);
        boost::shared_ptr<HomeAgentIndex> haIndex =
            haIndexController.getHomeAgentIndex(haAlias);
        if (haIndex) {
          string mapKey = ProtocolHelper::shortToString(
              dnsQueryMessage.getDNSHeader().getId());
          mapKey += "_" + dnsQueryMessage.getDNSQuestions()[0].getName();
          connection->getPendingRequests().add(mapKey, remoteEndpoint);
          boost::asio::ip::address_v4 remoteHAIp =
              boost::asio::ip::address_v4::from_string(haIndex->getIp());
          remoteEndpoint =
              boost::asio::ip::udp::endpoint(remoteHAIp, haIndex->getPort());
          LOG(INFO) << "[Thread " << std::hex << tid << "] "
                    << "Found home agent " << haIndex->getName().c_str()
                    << ", (" << haIndex->getIp().c_str()
                    << ":" << haIndex->getPort();
          this->forwardDNSMessage(dnsQueryMessage, remoteEndpoint, connection);
        } else
          isValid = false;
      }
    }
    if (!isValid) {
      DNSMessage reply;
      this->composeFailReply(dnsQueryMessage, reply);
      reply.print();
      this->forwardDNSMessage(dnsQueryMessage, remoteEndpoint, connection);
    }
  } else {
    string mapKey =
        ProtocolHelper::shortToString(dnsQueryMessage.getDNSHeader().getId());
    mapKey += "_" + dnsQueryMessage.getDNSAnswers()[0].getName();
    if (connection->getPendingRequests().lookup(mapKey, remoteEndpoint)) {
      connection->getPendingRequests().remove(mapKey);
      this->forwardDNSMessage(dnsQueryMessage, remoteEndpoint, connection);
    }
  }
}

void DNSMessageHandler::forwardDNSMessage(
    DNSMessage &message, boost::asio::ip::udp::endpoint &remoteEndpoint,
    boost::shared_ptr<UDPConnection> connection) {
  int n = message.getSize();
  unsigned long tid = static_cast<unsigned long>(pthread_self());
  LOG(INFO) << "[Thread " << std::hex << tid << "] "
            << "Forwarding " << message.getSize() << " bytes to "
            << remoteEndpoint.address().to_string().c_str()
            << ":" << remoteEndpoint.port();
  connection->getSocket().async_send_to(
      boost::asio::buffer(message.getBuffer(), message.getSize()),
      remoteEndpoint, boost::bind(&UDPConnection::handleDataSent, connection));
}

bool DNSMessageHandler::isValidRequest(DNSMessage &message,
                                       const string &suffix) {
  if (message.getDNSHeader().getOpcode() != O_QUERY)
    return false;
  int nQuestions = (int)message.getDNSQuestions().size();
  if (nQuestions <= 0)
    return false;
  for (int i = 0; i < nQuestions; i++) {
    DNSQuestion &question = message.getDNSQuestions()[i];
    if (question.getLabels().size() < 4)
      return false;
    if (!ProtocolHelper::isSuffix(question.getName(), suffix))
      return false;
  }
  return true;
}

void DNSMessageHandler::composeFailReply(DNSMessage &query, DNSMessage &reply) {
  unsigned short zero = 0;
  bool qr = true;
  ReturnCode retCode = R_NAME_ERROR;
  DNSResourceRecord ans;

  reply.setDNSHeader(query.getDNSHeader());
  reply.getDNSHeader().setQR(qr);
  reply.getDNSHeader().setANCount(zero);
  reply.getDNSHeader().setNSCount(zero);
  reply.getDNSHeader().setARCount(zero);
  reply.getDNSHeader().setQDCount(zero);
  reply.getDNSHeader().setRetCode(retCode);
  reply.allocateBuffer();
  reply.write();
}

void DNSMessageHandler::composeSuccessReply(DNSMessage &query,
                                            DNSMessage &reply,
                                            string &ansToQuestion) {
  unsigned short anCount = 1;
  unsigned short zero = 0;
  bool qr = true, aa = true;
  unsigned short nsCount = query.getDNSHeader().getNSCount();
  unsigned short arCount = query.getDNSHeader().getARCount();
  unsigned short qdCount = query.getDNSHeader().getQDCount();
  ReturnCode retCode = R_SUCCESS;
  QueryClass qClass = C_IN;
  QueryType qType = T_A;

  reply.setDNSHeader(query.getDNSHeader());
  reply.getDNSHeader().setAA(aa);
  reply.getDNSHeader().setQR(qr);
  reply.getDNSHeader().setANCount(anCount);
  reply.getDNSHeader().setNSCount(nsCount);
  reply.getDNSHeader().setARCount(arCount);
  reply.getDNSHeader().setQDCount(qdCount);

  for (int i = 0; i < nsCount; i++)
    reply.getDNSAuthority().push_back(query.getDNSAuthority()[i]);
  for (int i = 0; i < arCount; i++)
    reply.getDNSAdditional().push_back(query.getDNSAdditional()[i]);
  for (int i = 0; i < qdCount; i++)
    reply.getDNSQuestions().push_back(query.getDNSQuestions()[i]);
  reply.getDNSHeader().setRetCode(retCode);

  DNSResourceRecord ans;
  boost::array<unsigned char, sizeof(int)> ipAddressData =
      boost::asio::ip::address_v4::from_string(ansToQuestion).to_bytes();
  unsigned short rdLength = sizeof(int);
  unsigned int ttl = 0;
  ans.setLabels(query.getDNSQuestions()[0].getLabels());
  ans.setClass(qClass);
  ans.setType(qType);
  ans.setRdLength(rdLength);
  ans.setTtl(ttl);
  for (int j = 0; j < rdLength; j++)
    ans.getData().push_back(ipAddressData[j]);
  reply.getDNSAnswers().push_back(ans);
  reply.allocateBuffer();
  reply.write();
}
