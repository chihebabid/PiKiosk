//
// Created by chiheb on 31/08/2026.
//

#include "ManageMqttRec.h"


auto ManageMqttRec::message_arrived(mqtt::const_message_ptr msg) -> void {
    if (msg->get_topic()=="sensors/gas") {
        std::cout<<"gas receive: "<<msg->get_payload()<<'\n';
    }
}