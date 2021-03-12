/*
 * Copyright (c) 2020-2021 Arm Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#if MBED_CONF_AWS_CLIENT_SHADOW

#include "mbed.h"
#include "mbed-trace/mbed_trace.h"
#include "AWSClient/AWSClient.h"

extern "C" {
#include "core_json.h"
}

#define TRACE_GROUP "Main"

// This should not be called for the Device Shadow demo -
// Device Shadow messages are internally consumed by the client
void on_message_callback(
    const char *topic,
    uint16_t topic_length,
    const void *payload,
    size_t payload_length)
{
    tr_warning(
        "Message received on non-shadow topic: %.*s payload: %.*s",
        topic_length,
        topic,
        (const char *) payload_length,
        payload
    );
}

void demo()
{
    AWSClient &client = AWSClient::getInstance();

    // Download shadow document
    auto ret = client.downloadShadowDocument();
    if (ret == MBED_SUCCESS) {
        tr_info("Device Shadow document downloaded");
    } else {
        tr_error("AWSClient::downloadShadowDocument() failed: %d", ret);
        return; // cannot continue without downloadShadowDocument()
    }

    // Get a desired shadow value
    const char shadow_demo_int_key[] = "DemoNumber";
    const int shadow_demo_int_value = 100;
    char *shadow_value;
    size_t shadow_value_length;
    ret = client.getShadowDesiredValue(
        shadow_demo_int_key,
        strlen(shadow_demo_int_key),
        &shadow_value,
        &shadow_value_length
    );
    if (ret == MBED_SUCCESS) {
        tr_info("Desired value of %s: %.*s", shadow_demo_int_key, shadow_value_length, shadow_value);
    } else {
        tr_error(
            "AWSClient::getShadowDesiredValue() failed: %d, "
            "please ensure you have set a desired value for %s (e.g. using the AWS Console)",
            ret,
            shadow_demo_int_key
        );
    }

    // Report a string shadow value
    const char shadow_demo_string_key[] = "DemoName";
    const char shadow_demo_string_value[] = "mbed-os-example-for-aws";
    ret = client.publishShadowReportedValue(
        shadow_demo_string_key,
        strlen(shadow_demo_string_key),
        shadow_demo_string_value,
        strlen(shadow_demo_string_value)
    );
    if (ret == MBED_SUCCESS) {
        tr_info("Device Shadow reported string value published");
    } else {
        tr_error("AWSClient::publishShadowReportedValue() failed: %d", ret);
    }

    // Report an integer shadow value
    ret = client.publishShadowReportedValue(
        shadow_demo_int_key,
        strlen(shadow_demo_int_key),
        shadow_demo_int_value
    );
    if (ret == MBED_SUCCESS) {
        tr_info("Device Shadow reported integer value published");
    } else {
        tr_error("AWSClient::publishShadowReportedValue() failed: %d", ret);
    }
}

#endif // MBED_CONF_AWS_CLIENT_SHADOW