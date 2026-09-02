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
            gas_value_.reset();
        }
    }
    else if (msg->get_topic()=="commands") {
        if (msg->get_payload()=="SHOW_TEMP") {
            current_command_=Command::SHOW_TEMP;
        }
        else if (msg->get_payload()=="SHOW_GAS") {
            current_command_=Command::SHOW_GAS;
        }
        else {
            current_command_=Command::NONE;
        }
    }
}

auto ManageMqttRec::getGasValue() -> std::optional<uint32_t> {
    return gas_value_;
}

auto ManageMqttRec::getRefCurrentCommand() -> Command& {
    return current_command_;
}

std::optional<uint32_t> ManageMqttRec::gas_value_;
Command ManageMqttRec::current_command_;


