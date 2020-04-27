// AWS Certificates

#ifndef AWS_CREDENTIALS_H
#define AWS_CREDENTIALS_H

namespace aws {
namespace credentials {
/*
 * PEM-encoded root CA certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"
 * "...base64 data..."
 * "-----END CERTIFICATE-----";
 */
const char rootCA[] = "-----BEGIN CERTIFICATE-----\n"
"...\n"
"-----END CERTIFICATE-----";

/*
 * PEM-encoded client certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"
 * "...base64 data..."
 * "-----END CERTIFICATE-----";
 */
const char clientCrt[] = "-----BEGIN CERTIFICATE-----\n"
"...\n"
"-----END CERTIFICATE-----";

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----"
 * "...base64 data..."
 * "-----END RSA PRIVATE KEY-----";
 */
const char clientKey[] = "-----BEGIN RSA PRIVATE KEY-----\n"
"...\n"
"-----END RSA PRIVATE KEY-----";

}
}

#endif
