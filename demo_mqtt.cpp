/*
 * Copyright (c) 2020-2021 Arm Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#if !MBED_CONF_AWS_CLIENT_SHADOW

#include "mbed.h"
#include "mbed-trace/mbed_trace.h"
#include "rtos/ThisThread.h"
#include "AWSClient/AWSClient.h"

extern "C" {
#include "core_json.h"
}

#define TRACE_GROUP "Main"

static bool reply_received = false;

// Callback when a MQTT message has been added to the topic
void on_message_callback(
    const char *topic,
    uint16_t topic_length,
    const void *payload,
    size_t payload_length)
{
    char *json_value;
    size_t value_length;
    auto ret = JSON_Search((char *)payload, payload_length, "sender", strlen("sender"), &json_value, &value_length);
    if (ret == JSONSuccess && (strncmp(json_value, "device", strlen("device")) == 0)) {
        tr_info("Message sent successfully");
    } else {
        ret = JSON_Search((char *)payload, payload_length, "message", strlen("message"), &json_value, &value_length);
        if (ret == JSONSuccess) {
            reply_received = true;
            tr_info("Message received from the cloud: \"%.*s\"", value_length, json_value);
        } else {
            tr_error("Failed to extract message from the payload: \"%.*s\"", payload_length, (const char *) payload);
        }
    }
}

void demo()
{
    AWSClient &client = AWSClient::getInstance();

    // Subscribe to the topic
    const char topic[] = MBED_CONF_APP_AWS_MQTT_TOPIC;
    int ret = client.subscribe(topic, strlen(topic));
    if (ret != MBED_SUCCESS) {
        tr_error("AWSClient::subscribe() failed");
        return;
    }

    // Send ten message to the cloud (one per second)
    // Stop when we receive a cloud-to-device message
    char payload[128];
    for (int i = 0; i < 10; i++) {
        if (reply_received) {
            // If we have received a message from the cloud, don't send more messeges
            break;
        }

        // The MQTT protocol does not distinguish between senders,
        // so we add a "sender" attribute to the payload
        const char base_message[] = "messages left to send, or until we receive a reply";
        sprintf(payload, "{\n"
                "    \"sender\": \"device\",\n"
                "    \"message\": \"%d %s\"\n"
                "}",
                10 - i, base_message);
        tr_info("Publishing \"%d %s\" to topic \"%s\"", 10 - i, base_message, topic);
        ret = client.publish(
                  topic,
                  strlen(topic),
                  payload,
                  strlen(payload)
              );
        if (ret != MBED_SUCCESS) {
            tr_error("AWSClient::publish() failed");
            goto unsubscribe;
        }

        rtos::ThisThread::sleep_for(1s);
    }

    // If the user didn't manage to send a cloud-to-device message earlier,
    // let's wait until we receive one
    while (!reply_received) {
        // Continue to receive messages in the communication thread
        // which is internally created and maintained by the Azure SDK.
        sleep();
    }

unsubscribe:
    // Unsubscribe from the topic
    ret = client.unsubscribe(topic, strlen(topic));
    if (ret != MBED_SUCCESS) {
        tr_error("AWSClient::unsubscribe() failed");
    }
}

#endif // !MBED_CONF_AWS_CLIENT_SHADOW