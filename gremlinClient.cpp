#include "gremlinClient.hpp"
#include <iostream>
#include <functional>
using namespace std;

GremlinClient::GremlinClient(string wsURL, function<void(const string&)> callbackParam,
			     const std::string responseType)
  : callback(callbackParam), responseType(responseType) {
}

#ifndef EMSCRIPTEN
#include "easywsclient.hpp"
using easywsclient::WebSocket;


class CPPGremlinClient : public GremlinClient {
private:
  WebSocket::pointer ws;
  bool isReceived;

public:
  CPPGremlinClient(string wsURL, function<void(const string&)> callbackParam, const std::string responseType = TEXT)
    : GremlinClient(wsURL, callbackParam, responseType){
    this->ws = WebSocket::from_url(wsURL);
    if(ws == NULL)
      cerr << "Failed to open websocket at url : " << wsURL << endl;
    this->callback = callbackParam;
    isReceived = false;
  }

  ~CPPGremlinClient(){
    ws->close();
  }

  void send(string command) {
    string gremlin_request = "application/json|-{\"requestId\":\"655BD810-B41E-429D-B78F-3CC5F3B8E9BA\",\"processor\":\"\",\"op\":\"eval\",\"args\":{\"gremlin\":\"";
    gremlin_request += command;
    gremlin_request += "\"," + responseType + "}}";
    ws->send(gremlin_request);
    isReceived = false;
  }

  bool checkAnswer(int timeout) {
    ws->poll(timeout);
    ws->dispatch([&](const string & message) {
	isReceived = true;
	this->callback(message);
      });
    return isReceived;
  }
  
  void setCallback(function<void(const string&)> callbackParam) {
    this->callback = callbackParam;
  }

  void setResponseType(const string type) {
    if(type != TEXT && type !=JSON)
      cerr << "Invalid format for setResponseType : " << type << endl;
    responseType = type;
  }
};

#else
#include <emscripten/emscripten.h>
extern "C" {
  extern int createWebSocket(const char * wsURL);
  extern void sendJS(int wsId, const char * gremlinRequest);
  extern bool checkAnswerJS(int wsId, int timeout, char *answer);
  extern bool isRecvBufferEmpty();
}

class JSGremlinClient : public GremlinClient {
private:
  
  int wsId;
  string recvBuffer;
public:
  JSGremlinClient(string wsURL, function<void(const string&)> callbackParam, const std::string responseType = TEXT)
    : GremlinClient(wsURL, callbackParam, responseType){
    char * wsURLChar = new char[wsURL.size()];
    memcpy(wsURLChar, wsURL.c_str(), wsURL.size() * sizeof(char));
    this->wsId = createWebSocket(wsURLChar);
  };

  void setCallback(function<void(const string&)> callbackParam) {
    this->callback = callbackParam;
  }

  void send(string command) {
    string gremlin_request = "application/json|-{\"requestId\":\"655BD810-B41E-429D-B78F-3CC5F3B8E9BA\",\"processor\":\"\",\"op\":\"eval\",\"args\":{\"gremlin\":\"";
    gremlin_request += command;
    gremlin_request += "\"," + responseType + "}}";
    char * gremlin_requestChar = new char[gremlin_request.size()];
    memcpy(gremlin_requestChar, gremlin_request.c_str(), gremlin_request.size() * sizeof(char));
    sendJS(wsId, gremlin_requestChar);
  }

  bool checkAnswer(int timeout) {
    bool isReceived = false;
    while(!isRecvBufferEmpty()) {
      char * answer = new char[3000];
      isReceived = checkAnswerJS(wsId, timeout, answer);
      if(isReceived) {
	this->callback(answer);
      }
    }
    return isReceived;
  }

  void setResponseType(const string type) {
    if(type != TEXT && type !=JSON)
      cerr << "Invalid format for setResponseType : " << type << endl;
    else
      responseType = type;
  }
};

#endif

GremlinClient* GremlinClient::makeClient(string url, function<void(const string&)> callback, 
					 const string responseType) {
#ifndef EMSCRIPTEN
  return new CPPGremlinClient(url, callback, responseType);
#else
  return new JSGremlinClient(url, callback, responseType);
#endif
}
