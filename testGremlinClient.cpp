#include "gremlinClient.hpp"
#include <iostream>
#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif
using namespace std;

bool isReceived = true;
GremlinClient* client = NULL;

void handle_message(const std::string &message) {
  cout << "Message reçu : " << message << endl;
}

void one_iter() {
  string command;
  if(isReceived) {
    command = "";
    cin >> command;
    client->send(command);
  }
  isReceived = client->checkAnswer(5);
}

int main() {
  client = GremlinClient::makeClient("ws://localhost:8182", handle_message, TEXT);
#ifdef EMSCRIPTEN
  emscripten_set_main_loop(one_iter, 60, 1);
#else
  while (1) {
    one_iter();
  }
#endif
}



