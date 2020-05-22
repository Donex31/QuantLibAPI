#ifndef SERVICE_H_INCLUDED
#define SERVICE_H_INCLUDED
#include "Controller.h"

class Service : public Controller {
public:
    Service(const utility::string_t& address, const utility::string_t& port) : Controller(address, port) {}
    ~Service() {}
    void handlePost(http_request message);
    void initRestOpHandlers() override;
};
#endif // SERVICE_H_INCLUDED
