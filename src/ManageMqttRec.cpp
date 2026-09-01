//
// Created by chiheb on 31/08/2026.
//

#include "ManageMqttRec.h"
#include <numeric>


auto ManageMqttRec::message_arrived(mqtt::const_message_ptr msg) -> void {
    if (msg->get_topic()=="sensors/gas") {
        try {
            gas_value_.emplace(std::stoi(msg->get_payload()));
        }
        catch (const std::logic_error& e)
        {
            std::cerr<<"Cant convert\n";
            std::cerr<<"Payload"<<msg->get_payload()<<'\n';
            gas_value_.reset();
        }
    }
}

auto ManageMqttRec::getGasValue() -> std::optional<uint32_t> {
    return gas_value_;
}


std::optional<uint32_t> ManageMqttRec::gas_value_;



