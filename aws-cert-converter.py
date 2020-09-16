#!/usr/bin/env python

import argparse
from os import walk

# If modifying the template below,
# make sure to double escape newlines you want in the C-string (ie: '\\n')
# and to double-up brackets ('{' and '}' become '{{' and '}}') or else
# formatting of the template string will fail
header_template = (
    """
// AWS Certificates

#ifndef AWS_CREDENTIALS_H
#define AWS_CREDENTIALS_H

namespace aws {{
namespace credentials {{
/*
 * PEM-encoded root CA certificate
 */
const char rootCA[] = 
{0};

/*
 * PEM-encoded client certificate
 */
const char clientCrt[] = 
{1};

/*
 * PEM-encoded client private key.
 */
const char clientKey[] = 
{2};

}}
}}

#endif
    """
)

default_out_file_name = "aws_credentials.h"

# Formats the given file into a C-string with newlines
def format_file(file_path):
    out = ''
    with open(file_path, 'r') as f:
        lines = f.readlines()
        for l in lines:
            # Do not add a newline to the last line
            line_format = '"{}\\n"\n'
            if l is lines[-1]:
                line_format = '"{}"\n'
            out = out + line_format.format(l.strip())
    return out

def main():
    parser = argparse.ArgumentParser(description='Convert AWS credential files to a compatible C-style header format')
    parser.add_argument('certs_directory', metavar='d', help='Path to directory containing credential files downloaded from AWS IoT Console')
    global default_out_file_name
    parser.add_argument('--output-file', help='Output file name (optional, defaults to: {}'.format(default_out_file_name))
    args = parser.parse_args()

    # Walk the given directory to find the cert files we need
    f = []
    for (dirpath, dirnames, filenames) in walk(args.certs_directory):
        f.extend(filenames)
        break

    # These are the files we need, make sure they exist
    root_ca_file = None
    client_crt_file = None
    client_key_file = None
    for cert_file in f:
        if "root" in cert_file.lower():
            root_ca_file = args.certs_directory + "/" + cert_file
        elif ".pem.crt" in cert_file:
            client_crt_file = args.certs_directory + "/" + cert_file
        elif "private.pem.key" in cert_file:
            client_key_file = args.certs_directory + "/" + cert_file

    if root_ca_file:
        print("Root CA file found: {}".format(root_ca_file))
    else:
        print("Could not find Root CA file (eg: AmazonRootCA1.pem)!")
        exit()

    if client_crt_file:
        print("Client certificate file found: {}".format(client_crt_file))
    else:
        print("Could not find client certificate file (eg: xxxxxxxxxx-certificate.pem.crt)!")
        exit()

    if client_key_file:
        print("Client key file found: {}".format(client_key_file))
    else:
        print("Could not find client private key file (eg: xxxxxxxxxx-private.pem.key)!")
        exit()

    print("Generating C-style header from given credentials...")

    global header_template
    out_file_name = default_out_file_name
    if args.output_file:
        out_file_name = args.output_file

    with open(out_file_name, 'w+') as out:
        out.write(header_template.format(format_file(root_ca_file),\
                                         format_file(client_crt_file),\
                                         format_file(client_key_file)))
    
    print("Saved generated credentials header to {}!".format(out_file_name))

if __name__ == "__main__":
    main()
