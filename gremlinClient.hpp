#ifndef GREMLINCLIENT_H
#define GREMLINCLIENT_H


#include <functional>
#include <string>

const std::string TEXT = "\"accept\":\"text/plain\"";
const std::string JSON = "\"accept\":\"application/json\"";

class GremlinClient
{
public:
  GremlinClient(std::string wsURL, std::function<void(const std::string&)> callbackParam,
		const std::string responseType = TEXT);
  virtual ~GremlinClient() {};
  virtual void setCallback(std::function<void(const std::string&)> callback) = 0;
  virtual void send(std::string command) = 0;
  virtual bool checkAnswer(int timeout = 0) = 0;
  virtual void setResponseType(const std::string type) = 0;
  static GremlinClient* makeClient(std::string url, std::function<void(const std::string&)> callback,
				   const std::string responseType = TEXT);

protected:
  std::function<void(const std::string&)> callback;
  std::string responseType;
};

#endif
