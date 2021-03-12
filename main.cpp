/*
 * Copyright (c) 2020-2021 Arm Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "mbed-trace/mbed_trace.h"
#include "rtos/Mutex.h"
#include "rtos/Thread.h"
#include "rtos/ThisThread.h"
#include "AWSClient/AWSClient.h"
#include "aws_credentials.h"

extern "C" {
#include "core_json.h"
}

#define TRACE_GROUP "Main"

// Implemented by the two demos
void on_message_callback(
    const char *topic,
    uint16_t topic_length,
    const void *payload,
    size_t payload_length
);
void demo();

rtos::Mutex connection_mutex;

// Task to process MQTT responses at a regular interval
static void process_responses()
{
    AWSClient &client = AWSClient::getInstance();
    while (true) {
        connection_mutex.lock(); // avoid AWSClient::disconnect() while processing
        if (!client.isConnected()) {
            connection_mutex.unlock();
            break;
        }
        if (client.processResponses() != MBED_SUCCESS) {
            tr_error("AWSClient::processResponses() failed");
        }
        connection_mutex.unlock();
        rtos::ThisThread::sleep_for(10ms);
    }
}

int main()
{
    // "goto" requires early initialization of variables
    AWSClient &client = AWSClient::getInstance();
    rtos::Thread process_thread;
    AWSClient::TLSCredentials_t credentials;
    int ret;

    mbed_trace_init();
    tr_info("Connecting to the network...");
    auto network = NetworkInterface::get_default_instance();
    if (network == NULL) {
        tr_error("No network interface found");
        goto end;
    }
    ret = network->connect();
    if (ret != 0) {
        tr_error("Connection error: %x", ret);
        goto end;
    }
    tr_info("MAC: %s", network->get_mac_address());
    tr_info("Connection Success");

    // Set credentials
    credentials.clientCrt = aws::credentials::clientCrt;
    credentials.clientCrtLen = sizeof(aws::credentials::clientCrt);
    credentials.clientKey = aws::credentials::clientKey;
    credentials.clientKeyLen = sizeof(aws::credentials::clientKey);
    credentials.rootCrtMain = aws::credentials::rootCA;
    credentials.rootCrtMainLen = sizeof(aws::credentials::rootCA);

    // Initialize client
    ret = client.init(
              on_message_callback,
              credentials
          );
    if (ret != MBED_SUCCESS) {
        tr_error("AWSClient::init() failed");
        goto disconnect;
    }

    // Connect to AWS IoT Core
    ret = client.connect(
              network,
              credentials,
              MBED_CONF_APP_AWS_ENDPOINT,
              MBED_CONF_APP_AWS_CLIENT_IDENTIFIER
          );
    if (ret != MBED_SUCCESS) {
        tr_error("AWSClient::connect() failed");
        goto disconnect;
    }

    // Start a background thread to process MQTT
    ret = process_thread.start(process_responses);
    if (ret != osOK) {
        tr_error("Failed to start thread to process MQTT");
        goto disconnect;
    }

    // Run a demo depending on the configuration in mbed_app.json:
    // * demo_mqtt.cpp if aws-client.shadow is unset or false
    // * demo_shadow.cpp if aws-client.shadow is true
    demo();

disconnect:
    if (client.isConnected()) {
        connection_mutex.lock();
        ret = client.disconnect();
        connection_mutex.unlock();
        if (ret != MBED_SUCCESS) {
            tr_error("AWS::disconnect() failed");
        }
    }

    // The process thread should finish on disconnection
    ret = process_thread.join();
    if (ret != osOK) {
        tr_error("Failed to join thread");
    }

    ret = network->disconnect();
    if (ret != MBED_SUCCESS) {
        tr_error("NetworkInterface::disconnect() failed");
    }

end:
    tr_info("End of the demo");
}