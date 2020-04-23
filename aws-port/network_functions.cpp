#include "TLSSocket.h"
#include "NetworkInterface.h"
#include "mbed_trace.h"
#define TRACE_GROUP "AWSPort"

extern "C" {

//#include "aws_iot_error.h"
// This is superduperdirty
// This is done because SUCCESS & FAILURE colide with the definition from ST's SDK
#define AWS_IOT_SDK_SRC_IOT_ERROR_H_
typedef enum {
	/** Returned when the Network physical layer is connected */
			NETWORK_PHYSICAL_LAYER_CONNECTED = 6,
	/** Returned when the Network is manually disconnected */
			NETWORK_MANUALLY_DISCONNECTED = 5,
	/** Returned when the Network is disconnected and the reconnect attempt is in progress */
			NETWORK_ATTEMPTING_RECONNECT = 4,
	/** Return value of yield function to indicate auto-reconnect was successful */
			NETWORK_RECONNECTED = 3,
	/** Returned when a read attempt is made on the TLS buffer and it is empty */
			MQTT_NOTHING_TO_READ = 2,
	/** Returned when a connection request is successful and packet response is connection accepted */
			MQTT_CONNACK_CONNECTION_ACCEPTED = 1,
	/** Success return value - no error occurred */
			AWS_SUCCESS = 0,
	/** A generic error. Not enough information for a specific error code */
			AWS_FAILURE = -1,
	/** A required parameter was passed as null */
			NULL_VALUE_ERROR = -2,
	/** The TCP socket could not be established */
			TCP_CONNECTION_ERROR = -3,
	/** The TLS handshake failed */
			SSL_CONNECTION_ERROR = -4,
	/** Error associated with setting up the parameters of a Socket */
			TCP_SETUP_ERROR = -5,
	/** A timeout occurred while waiting for the TLS handshake to complete. */
			NETWORK_SSL_CONNECT_TIMEOUT_ERROR = -6,
	/** A Generic write error based on the platform used */
			NETWORK_SSL_WRITE_ERROR = -7,
	/** SSL initialization error at the TLS layer */
			NETWORK_SSL_INIT_ERROR = -8,
	/** An error occurred when loading the certificates.  The certificates could not be located or are incorrectly formatted. */
			NETWORK_SSL_CERT_ERROR = -9,
	/** SSL Write times out */
			NETWORK_SSL_WRITE_TIMEOUT_ERROR = -10,
	/** SSL Read times out */
			NETWORK_SSL_READ_TIMEOUT_ERROR = -11,
	/** A Generic error based on the platform used */
			NETWORK_SSL_READ_ERROR = -12,
	/** Returned when the Network is disconnected and reconnect is either disabled or physical layer is disconnected */
			NETWORK_DISCONNECTED_ERROR = -13,
	/** Returned when the Network is disconnected and the reconnect attempt has timed out */
			NETWORK_RECONNECT_TIMED_OUT_ERROR = -14,
	/** Returned when the Network is already connected and a connection attempt is made */
			NETWORK_ALREADY_CONNECTED_ERROR = -15,
	/** Network layer Error Codes */
	/** Network layer Random number generator seeding failed */
			NETWORK_MBEDTLS_ERR_CTR_DRBG_ENTROPY_SOURCE_FAILED = -16,
	/** A generic error code for Network layer errors */
			NETWORK_SSL_UNKNOWN_ERROR = -17,
	/** Returned when the physical layer is disconnected */
			NETWORK_PHYSICAL_LAYER_DISCONNECTED = -18,
	/** Returned when the root certificate is invalid */
			NETWORK_X509_ROOT_CRT_PARSE_ERROR = -19,
	/** Returned when the device certificate is invalid */
			NETWORK_X509_DEVICE_CRT_PARSE_ERROR = -20,
	/** Returned when the private key failed to parse */
			NETWORK_PK_PRIVATE_KEY_PARSE_ERROR = -21,
	/** Returned when the network layer failed to open a socket */
			NETWORK_ERR_NET_SOCKET_FAILED = -22,
	/** Returned when the server is unknown */
			NETWORK_ERR_NET_UNKNOWN_HOST = -23,
	/** Returned when connect request failed */
			NETWORK_ERR_NET_CONNECT_FAILED = -24,
	/** Returned when there is nothing to read in the TLS read buffer */
			NETWORK_SSL_NOTHING_TO_READ = -25,
	/** A connection could not be established. */
			MQTT_CONNECTION_ERROR = -26,
	/** A timeout occurred while waiting for the TLS handshake to complete */
			MQTT_CONNECT_TIMEOUT_ERROR = -27,
	/** A timeout occurred while waiting for the TLS request complete */
			MQTT_REQUEST_TIMEOUT_ERROR = -28,
	/** The current client state does not match the expected value */
			MQTT_UNEXPECTED_CLIENT_STATE_ERROR = -29,
	/** The client state is not idle when request is being made */
			MQTT_CLIENT_NOT_IDLE_ERROR = -30,
	/** The MQTT RX buffer received corrupt or unexpected message  */
			MQTT_RX_MESSAGE_PACKET_TYPE_INVALID_ERROR = -31,
	/** The MQTT RX buffer received a bigger message. The message will be dropped  */
			MQTT_RX_BUFFER_TOO_SHORT_ERROR = -32,
	/** The MQTT TX buffer is too short for the outgoing message. Request will fail  */
			MQTT_TX_BUFFER_TOO_SHORT_ERROR = -33,
	/** The client is subscribed to the maximum possible number of subscriptions  */
			MQTT_MAX_SUBSCRIPTIONS_REACHED_ERROR = -34,
	/** Failed to decode the remaining packet length on incoming packet */
			MQTT_DECODE_REMAINING_LENGTH_ERROR = -35,
	/** Connect request failed with the server returning an unknown error */
			MQTT_CONNACK_UNKNOWN_ERROR = -36,
	/** Connect request failed with the server returning an unacceptable protocol version error */
			MQTT_CONNACK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR = -37,
	/** Connect request failed with the server returning an identifier rejected error */
			MQTT_CONNACK_IDENTIFIER_REJECTED_ERROR = -38,
	/** Connect request failed with the server returning an unavailable error */
			MQTT_CONNACK_SERVER_UNAVAILABLE_ERROR = -39,
	/** Connect request failed with the server returning a bad userdata error */
			MQTT_CONNACK_BAD_USERDATA_ERROR = -40,
	/** Connect request failed with the server failing to authenticate the request */
			MQTT_CONNACK_NOT_AUTHORIZED_ERROR = -41,
	/** An error occurred while parsing the JSON string.  Usually malformed JSON. */
			JSON_PARSE_ERROR = -42,
	/** Shadow: The response Ack table is currently full waiting for previously published updates */
			SHADOW_WAIT_FOR_PUBLISH = -43,
	/** Any time an snprintf writes more than size value, this error will be returned */
			SHADOW_JSON_BUFFER_TRUNCATED = -44,
	/** Any time an snprintf encounters an encoding error or not enough space in the given buffer */
			SHADOW_JSON_ERROR = -45,
	/** Mutex initialization failed */
			MUTEX_INIT_ERROR = -46,
	/** Mutex lock request failed */
			MUTEX_LOCK_ERROR = -47,
	/** Mutex unlock request failed */
			MUTEX_UNLOCK_ERROR = -48,
	/** Mutex destroy failed */
			MUTEX_DESTROY_ERROR = -49,
	/** Input argument exceeded the allowed maximum size */
			MAX_SIZE_ERROR = -50,
	/** Some limit has been exceeded, e.g. the maximum number of subscriptions has been reached */
			LIMIT_EXCEEDED_ERROR = -51,
	/** Invalid input topic type */
			INVALID_TOPIC_TYPE_ERROR = -52
} IoT_Error_t;
#include "aws_iot_log.h"
#include "network_platform.h"
#include "network_interface.h"
#include "timer_platform.h"

static void _iot_tls_set_connect_params(Network *pNetwork, char *pRootCALocation, char *pDeviceCertLocation,
								 char *pDevicePrivateKeyLocation, char *pDestinationURL,
								 uint16_t destinationPort, uint32_t timeout_ms, bool ServerVerificationFlag) {
	pNetwork->tlsConnectParams.DestinationPort = destinationPort;
	pNetwork->tlsConnectParams.pDestinationURL = pDestinationURL;
	pNetwork->tlsConnectParams.pDeviceCertLocation = pDeviceCertLocation;
	pNetwork->tlsConnectParams.pDevicePrivateKeyLocation = pDevicePrivateKeyLocation;
	pNetwork->tlsConnectParams.pRootCALocation = pRootCALocation;
	pNetwork->tlsConnectParams.timeout_ms = timeout_ms;
	pNetwork->tlsConnectParams.ServerVerificationFlag = ServerVerificationFlag;
}

IoT_Error_t iot_tls_init(Network *pNetwork, char *pRootCALocation, char *pDeviceCertLocation,
        char *pDevicePrivateKeyLocation, char *pDestinationURL, uint16_t destinationPort,
        uint32_t timeout_ms, bool ServerVerificationFlag)
{
    FUNC_ENTRY;
	_iot_tls_set_connect_params(pNetwork, pRootCALocation, pDeviceCertLocation, pDevicePrivateKeyLocation,
								pDestinationURL, destinationPort, timeout_ms, ServerVerificationFlag);

	pNetwork->connect = iot_tls_connect;
	pNetwork->read = iot_tls_read;
	pNetwork->write = iot_tls_write;
	pNetwork->disconnect = iot_tls_disconnect;
	pNetwork->isConnected = iot_tls_is_connected;
	pNetwork->destroy = iot_tls_destroy;

    pNetwork->tlsDataParams.socket = NULL;

    FUNC_EXIT_RC(AWS_SUCCESS);
}

IoT_Error_t iot_tls_is_connected(Network *pNetwork) {
    FUNC_ENTRY;
    IoT_Error_t result;
	/* Use this to add implementation which can check for physical layer disconnect */
    if (NSAPI_STATUS_GLOBAL_UP == NetworkInterface::get_default_instance()->get_connection_status()) {
	    result = NETWORK_PHYSICAL_LAYER_CONNECTED;
    } else {
        result = NETWORK_PHYSICAL_LAYER_DISCONNECTED;
    }
    FUNC_EXIT_RC(result);
}

IoT_Error_t iot_tls_connect(Network *pNetwork, TLSConnectParams *params) {
    FUNC_ENTRY;
	if(NULL == pNetwork) {
        FUNC_EXIT_RC(NULL_VALUE_ERROR);
	}

	if(NULL != params) {
		_iot_tls_set_connect_params(pNetwork, params->pRootCALocation, params->pDeviceCertLocation,
									params->pDevicePrivateKeyLocation, params->pDestinationURL,
									params->DestinationPort, params->timeout_ms, params->ServerVerificationFlag);
	}

    auto socket = new TLSSocket();
    if (socket == NULL) {
        tr_error("failed to create socket");
        FUNC_EXIT_RC(SSL_CONNECTION_ERROR); // TODO use the appropriate error code
    }
    auto net = NetworkInterface::get_default_instance();
    socket->open(net);

    socket->set_root_ca_cert(pNetwork->tlsConnectParams.pRootCALocation);
    socket->set_client_cert_key(pNetwork->tlsConnectParams.pDeviceCertLocation,
                                pNetwork->tlsConnectParams.pDevicePrivateKeyLocation);

    // if port 443 alpn protocol is requested.
    if (443 == pNetwork->tlsConnectParams.DestinationPort) {
        auto ssl_config = socket->get_ssl_config();
        static const char *alpnProtocols[] = {"x-amzn-mqtt-ca", NULL};
        if (0 != mbedtls_ssl_conf_alpn_protocols(ssl_config, alpnProtocols)) {
            tr_error("Failed to set alpn");
            delete socket;
            FUNC_EXIT_RC(SSL_CONNECTION_ERROR);
        }
    }

    socket->set_hostname(pNetwork->tlsConnectParams.pDestinationURL);

    SocketAddress addr;
    auto result = net->gethostbyname(pNetwork->tlsConnectParams.pDestinationURL, &addr);
    if (result != NSAPI_ERROR_OK) {
        tr_error("Error! DNS resolution for %s failed with %d",
                 pNetwork->tlsConnectParams.pDestinationURL, result);
    }
    addr.set_port(pNetwork->tlsConnectParams.DestinationPort);

    nsapi_error_t res = socket->connect(addr);
    if (NSAPI_ERROR_OK != res) {
        tr_error("failed to connect with : %d", res);
        delete socket;
        FUNC_EXIT_RC(SSL_CONNECTION_ERROR);
    };
    
    pNetwork->tlsDataParams.socket = socket;
    FUNC_EXIT_RC(AWS_SUCCESS);
}

IoT_Error_t iot_tls_write(Network *pNetwork, unsigned char *pMsg, size_t len, Timer *timer, size_t *written_len) {
    FUNC_ENTRY;
    auto socket = static_cast<TLSSocket *>(pNetwork->tlsDataParams.socket);
    auto written_so_far = 0;
    auto isErrorFlag = false;
    auto ret = 0;

    socket->set_blocking(false);
	for(written_so_far = 0;
		written_so_far < len && !has_timer_expired(timer); written_so_far += ret) {
		while(!has_timer_expired(timer) &&
			  (ret = socket->send(pMsg + written_so_far, len - written_so_far)) <= 0) {
			if((ret < 0) && (ret != NSAPI_ERROR_WOULD_BLOCK)) {
				tr_error("TLSSocket::send returned -0x%x", -ret);
				/* All other negative return values indicate connection needs to be reset.
		 		* Will be caught in ping request so ignored here */
				isErrorFlag = true;
				break;
			}
		}
		if(isErrorFlag) {
			break;
		}
	}
    socket->set_blocking(true);

	*written_len = written_so_far;

	if(isErrorFlag) {
		FUNC_EXIT_RC(NETWORK_SSL_WRITE_ERROR);
	} else if(has_timer_expired(timer) && written_so_far != len) {
		FUNC_EXIT_RC(NETWORK_SSL_WRITE_TIMEOUT_ERROR);
	}

    FUNC_EXIT_RC(AWS_SUCCESS);
}

IoT_Error_t iot_tls_read(Network *pNetwork, unsigned char *pMsg, size_t len, Timer *timer, size_t *read_len) {
    FUNC_ENTRY;

    auto socket = static_cast<TLSSocket *>(pNetwork->tlsDataParams.socket);
    size_t rxLen = 0;
	int ret;

    socket->set_blocking(false);
	while (len > 0) {
		// This read will timeout after IOT_SSL_READ_TIMEOUT if there's no data to be read
		ret = socket->recv(pMsg, len);
		if (ret > 0) {
			rxLen += ret;
			pMsg += ret;
			len -= ret;
		} else if ((ret < 0) && (ret != NSAPI_ERROR_WOULD_BLOCK)) {
            tr_error("failed ! TLSSocket::recv returned -0x%x", -ret);
			FUNC_EXIT_RC(NETWORK_SSL_READ_ERROR);
		}

		// Evaluate timeout after the read to make sure read is done at least once
		if (has_timer_expired(timer)) {
			break;
		}
	}
    socket->set_blocking(true);

	if (len == 0) {
		*read_len = rxLen;
		FUNC_EXIT_RC(AWS_SUCCESS);
	}

	if (rxLen == 0) {
		FUNC_EXIT_RC(NETWORK_SSL_NOTHING_TO_READ);
	} else {
		FUNC_EXIT_RC(NETWORK_SSL_READ_TIMEOUT_ERROR);
	}
}

IoT_Error_t iot_tls_disconnect(Network *pNetwork) {
    FUNC_ENTRY;
    if (pNetwork->tlsDataParams.socket != NULL) {
        auto socket = static_cast<TLSSocket *>(pNetwork->tlsDataParams.socket);
        socket->close();
    }

	FUNC_EXIT_RC(AWS_SUCCESS);
}

IoT_Error_t iot_tls_destroy(Network *pNetwork) {
    FUNC_ENTRY;
    if (pNetwork->tlsDataParams.socket != NULL) {
        delete static_cast<TLSSocket *>(pNetwork->tlsDataParams.socket);
    }

	FUNC_EXIT_RC(AWS_SUCCESS);
}

} /* extern "C" */

