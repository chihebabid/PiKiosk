//
// Created by chiheb on 31/08/2026.
//

#ifndef KIOSK_APP_MANAGEMQTTREC_H
#define KIOSK_APP_MANAGEMQTTREC_H
#include <mqtt/callback.h>

class ManageMqttRec : public mqtt::callback {
public:
    void message_arrived(mqtt::const_message_ptr msg) override;
    static auto getGasValue() -> std::optional<uint32_t>;
private:
    static std::optional<uint32_t> gas_value_;
};


#endif //KIOSK_APP_MANAGEMQTTREC_H
