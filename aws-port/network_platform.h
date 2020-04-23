
#ifndef AWS_PORT_NETWORK_PLATFORM_H
#define AWS_PORT_NETWORK_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief TLS Connection Parameters
 *
 * Defines a type containing TLS specific parameters to be passed down to the
 * TLS networking layer to create a TLS secured socket.
 */
typedef struct _TLSDataParams {
    void *socket;
}TLSDataParams;

#ifdef __cplusplus
}
#endif
#endif /*AWS_PORT_NETWORK_PLATFORM_H*/
