//
// Created by chiheb on 31/08/2026.
//

#ifndef KIOSK_APP_MANAGEMQTTREC_H
#define KIOSK_APP_MANAGEMQTTREC_H
#include "misc.h"
#include <mqtt/callback.h>

class ManageMqttRec : public mqtt::callback {
public:
    auto message_arrived(mqtt::const_message_ptr msg) -> void override;
    static auto getGasValue() -> std::optional<uint32_t>;
    static auto getCurrentCommand() -> Command;
    static auto resetCommand() -> void;
private:
    static std::optional<uint32_t> gas_value_;
    static Command current_command_;
};


#endif //KIOSK_APP_MANAGEMQTTREC_H
