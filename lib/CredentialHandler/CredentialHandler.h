#ifndef CREDENTIAL_HANDLER_H
#define CREDENTIAL_HANDLER_H

#include <string>

struct Credentials {
    std::string mqtt_token;
    std::string wifi_ssid;
    std::string wifi_password;
};

class CredentialHandler {
public:
    static Credentials read_credentials(const std::string& file_path);
};

#endif // CREDENTIAL_HANDLER_H
