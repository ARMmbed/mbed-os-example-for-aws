#include "mbed.h"
#include "mbed_trace.h"
#include "mbedtls/debug.h"
#include "aws_credentials.h"

extern "C" {
// sdk initialization
#include "iot_init.h"
// mqtt methods
#include "iot_mqtt.h"
}

#define TRACE_GROUP "Main"

#if TARGET_DISCO_L475VG_IOT01A
NetworkInterface *NetworkInterface::get_default_instance() {
    return WiFiInterface::get_default_instance();
}
#endif
static Mutex trace_mutex;
static void my_mutex_wait()
{
    trace_mutex.lock();
}
static void my_mutex_release()
{
    trace_mutex.unlock();
}
extern "C" void synchronized_log(const char *msg) {
    my_mutex_wait();
    puts(msg);
    my_mutex_release();
}

#define MQTT_TIMEOUT_MS    15000

static void on_message_received(void * pCallbackContext, IotMqttCallbackParam_t *pCallbackParam) {
    auto wait_sem = static_cast<Semaphore*>(pCallbackContext);
    char* payload = (char*)pCallbackParam->u.message.info.pPayload;
    auto payloadLen = pCallbackParam->u.message.info.payloadLength;
    tr_debug("from topic:%s; msg: %.*s", pCallbackParam->u.message.info.pTopicName, payloadLen, payload);

    if (strncmp("Warning", payload, 7) != 0) {
        tr_info("Hello %.*s !", payloadLen, payload);
        wait_sem->release();
    }
}
int main()
{
    mbed_trace_mutex_wait_function_set( my_mutex_wait ); // only if thread safety is needed
    mbed_trace_mutex_release_function_set( my_mutex_release ); // only if thread safety is needed
    mbed_trace_init();

#if TARGET_DISCO_L475VG_IOT01A
    WiFiInterface *wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        tr_error("No WiFiInterface found.");
        return -1;
    }

    tr_info("Connecting to %s...", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    tr_info("MAC: %s", wifi->get_mac_address());
#else
    tr_info("Connecting to ethernet...");
    auto eth = NetworkInterface::get_default_instance();
    if (eth == NULL) {
        tr_error("No NetworkInterface found.");
        return -1;
    }
    int ret = eth->connect();
    tr_info("MAC: %s", eth->get_mac_address());
#endif
    if (ret != 0) {
        tr_error("Connection error: %x", ret);
        return -1;
    }

    tr_info("Connection Success");

    // demo :
    // - Init sdk
    if (!IotSdk_Init()) {
        tr_error("AWS Sdk: failed to initialize IotSdk");
        while(true);
    }
    auto init_status = IotMqtt_Init();
    if (init_status != IOT_MQTT_SUCCESS) {
        tr_error("AWS Sdk: Failed to initialize IotMqtt with %u", init_status);
        while(true);
    }
    // - Connect to mqtt broker
    IotMqttNetworkInfo_t network_info = IOT_MQTT_NETWORK_INFO_INITIALIZER;
    network_info.pNetworkInterface = aws::get_iot_network_interface();
    // create nework connection
    network_info.createNetworkConnection = true;
    network_info.u.setup.pNetworkServerInfo = {
        .hostname = MBED_CONF_APP_AWS_ENDPOINT,
        .port = 8883
    };
    network_info.u.setup.pNetworkCredentialInfo = {
        .rootCA = aws::credentials::rootCA,
        .clientCrt = aws::credentials::clientCrt,
        .clientKey = aws::credentials::clientKey
    };

    IotMqttConnectInfo_t connect_info = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    connect_info.awsIotMqttMode = true; // we are connecting to aws servers
    connect_info.pClientIdentifier = MBED_CONF_APP_AWS_CLIENT_IDENTIFIER;
    connect_info.clientIdentifierLength = strlen(MBED_CONF_APP_AWS_CLIENT_IDENTIFIER);

    IotMqttConnection_t connection = IOT_MQTT_CONNECTION_INITIALIZER;
    auto connect_status = IotMqtt_Connect(&network_info, &connect_info, /* timeout ms */ MQTT_TIMEOUT_MS, &connection);
    if (connect_status != IOT_MQTT_SUCCESS) {
        tr_error("AWS Sdk: Connection to the MQTT broker failed with %u", connect_status);
        while(true);
    }
    // - Subscribe to sdkTest/sub
    //   On message
    //   - Display on the console: "Hello %s", message
    /* Set the members of the subscription. */
    static const char topic[] = MBED_CONF_APP_AWS_MQTT_TOPIC;
    Semaphore wait_sem {/* count */ 0, /* max_count */ 1};

    IotMqttSubscription_t subscription = IOT_MQTT_SUBSCRIPTION_INITIALIZER;
    subscription.qos = IOT_MQTT_QOS_1;
    subscription.pTopicFilter = topic;
    subscription.topicFilterLength = strlen(topic);
    subscription.callback.function = on_message_received;
    subscription.callback.pCallbackContext = &wait_sem;

    /* Subscribe to the topic using the blocking SUBSCRIBE
     * function. */
    auto sub_status = IotMqtt_SubscribeSync(connection, &subscription,
                                            /* subscription count */ 1, /* flags */ 0,
                                            /* timeout ms */ MQTT_TIMEOUT_MS );
    if (sub_status != IOT_MQTT_SUCCESS) {
        tr_error("AWS Sdk: Subscribe failed with : %u", sub_status);
    }

    /* Set the members of the publish info. */
    IotMqttPublishInfo_t publish = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    publish.qos = IOT_MQTT_QOS_1;
    publish.pTopicName = topic;
    publish.topicNameLength = strlen(topic);
    publish.retryLimit = 3;
    publish.retryMs = 1000;
    for (uint32_t i = 0; i < 10; i++) {
        // - for i in 0..9
        //  - wait up to 1 sec
        //  - if no message received Publish: "You have %d sec remaining to say hello...", 10-i
        //  - other wise, exit
        if (wait_sem.try_acquire_for(1000)) {
            break;
        }

        /* prepare the message */
        static char message[64];
        snprintf(message, 64, "Warning: Only %lu seconde(s) left to say your name !", 10 - i);
        publish.pPayload = message;
        publish.payloadLength = strlen(message);

        /* Publish the message. */
        auto pub_status = IotMqtt_PublishSync(connection, &publish,
                                              /* flags */ 0, /* timeout ms */ MQTT_TIMEOUT_MS);
        if (pub_status != IOT_MQTT_SUCCESS) {
            tr_warning("AWS Sdk: failed to publish message with %u.", pub_status);
        }
    }

    /* Close the MQTT connection. */
    IotMqtt_Disconnect(connection, 0);

    IotMqtt_Cleanup();
    IotSdk_Cleanup();

    tr_info("Done");
    while (true) {
        ThisThread::sleep_for(1000);
    }
    return 0;
}
