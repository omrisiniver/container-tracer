#pragma once

#include "yhirose/httplib.h"

class HttpCommunicator {
public:
    HttpCommunicator(const char* url)
      : m_client(url) 
    { }

private:
    httplib::Client m_client;
};