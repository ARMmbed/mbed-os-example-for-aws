/*
 * AWS Certificates
 * Copyright (c) 2020 Arm Limited
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef AWS_CREDENTIALS_H
#define AWS_CREDENTIALS_H

namespace aws {
namespace credentials {
/*
 * PEM-encoded root CA certificate
 *
 * Must include the PEM header and footer,
 * and every line of the body needs to be quoted and end with \n:
 * "-----BEGIN CERTIFICATE-----\n"
 * "...base64 data...\n"
 * "-----END CERTIFICATE-----";
 */
const char rootCA[] = "-----BEGIN CERTIFICATE-----\n"
"...\n"
"...\n"
"...\n"
"-----END CERTIFICATE-----";

/*
 * PEM-encoded client certificate
 *
 * Must include the PEM header and footer,
 * and every line of the body needs to be quoted and end with \n:
 * "-----BEGIN CERTIFICATE-----\n"
 * "...base64 data...\n"
 * "-----END CERTIFICATE-----";
 */
const char clientCrt[] = "-----BEGIN CERTIFICATE-----\n"
"...\n"
"...\n"
"...\n"
"-----END CERTIFICATE-----";

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer,
 * and every line of the body needs to be quoted and end with \n:
 * "-----BEGIN RSA PRIVATE KEY-----\n"
 * "...base64 data...\n"
 * "-----END RSA PRIVATE KEY-----";
 */
const char clientKey[] = "-----BEGIN RSA PRIVATE KEY-----\n"
"...\n"
"...\n"
"...\n"
"-----END RSA PRIVATE KEY-----";

}
}

#endif
