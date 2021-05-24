![](./resources/official_armmbed_example_badge.png)

# Mbed OS example for AWS cloud

The example project is part of the [Arm Mbed OS Official Examples](https://os.mbed.com/code/). It contains two demos:
* MQTT (default): publishes a message every 1 second for 10 messages or until a message is received.
* Device Shadow service: retrieves a desired value from the cloud, then reports a string value and then an integer value to the cloud.

You can build the project with all supported [Mbed OS build tools](https://os.mbed.com/docs/mbed-os/latest/tools/index.html). However, this example project specifically refers to the command-line interface tool [Arm Mbed CLI](https://github.com/ARMmbed/mbed-cli#installing-mbed-cli).
(Note: To see a rendered example you can import into the Arm Online Compiler, please see our [import quick start](https://os.mbed.com/docs/mbed-os/latest/quick-start/online-with-the-online-compiler.html#importing-the-code).)

## Mbed OS build tools

### Mbed CLI 2
Starting with version 6.5, Mbed OS uses Mbed CLI 2. It uses Ninja as a build system, and CMake to generate the build environment and manage the build process in a compiler-independent manner. If you are working with Mbed OS version prior to 6.5 then check the section [Mbed CLI 1](#mbed-cli-1).
1. [Install Mbed CLI 2](https://os.mbed.com/docs/mbed-os/latest/build-tools/install-or-upgrade.html).
1. From the command-line, import the example: `mbed-tools import mbed-os-example-for-aws`
1. Change the current directory to where the project was imported.

### Mbed CLI 1
1. [Install Mbed CLI 1](https://os.mbed.com/docs/mbed-os/latest/quick-start/offline-with-mbed-cli.html).
1. From the command-line, import the example: `mbed import mbed-os-example-for-aws`
1. Change the current directory to where the project was imported.

## Configuring the AWS IoT Core service

1. Create an AWS account if you don't have one, and login to it.

    __NOTE:__ If you have an admin for your AWS account, please contact them to add a user to the account. You should obtain your login credentials from your admin in this case.

1. Set up device credentials and policy via the AWS IoT console. You can refer to the AWS documentation [here](https://docs.aws.amazon.com/iot/latest/developerguide/iot-gs.html). Follow the steps there to

    * create a thing
    * generate a device certificate and keys
    * create an IoT policy and attach that policy to your device.

    Also download "Amazon Root CA 1" from [here](https://docs.aws.amazon.com/iot/latest/developerguide/server-authentication.html#server-authentication-certs).

    Once you have downloaded the credentials, you will need to place them in the [`aws_credentials.h`](./aws_credentials.h) file of this example.
    
    The example includes a python script to automate converting the credentials you downloaded from AWS into C-compatible arrays/strings. First, create a new folder in the project to store your credential files, eg: `mkdir aws-credentials`. Copy the previously-downloaded credential files into this subdirectory.
    
    Then, you can run the script to automatically generate the necessary code from the credentials:
    
    ```
    python aws-cert-converter.py aws-credentials
    ```
    
    For more details on how to use the convert script, simply pass in the `-h` flag to print the help documentation.
    
    The above command will read your credential files and place them into a C header file for you: `aws_credentials.h`

1. Once you have created the "thing", you will need to obtain the custom endpoint URL from the console. At the time of writing this document, you can find it under "Settings" in the IoT console.

    In [`mbed_app.json`](./mbed_app.json) file, set `aws-endpoint.value` to be that of the custom endpoint.

1. Set a topic that both your device and the cloud can publish messages to.

   In [`mbed_app.json`](./mbed_app.json) file, set `aws-mqtt-topic.value` to a name you prefer, or use the default one. On the AWS console, you will then need to subscribe to the same topic name. At the time of writing this document, you can find this under "Test" on the console.

1. Give your device a name by setting `aws-client-identifier` in [`mbed_app.json`](./mbed_app.json).

## Building and running

### MQTT demo (default)

1. If using WiFi (e.g. on DISCO_L475VG_IOT01A), enter your network's SSID and password in [`mbed_app.json`](./mbed_app.json) (see [here](https://github.com/ARMmbed/mbed-os-example-wifi/blob/master/README.md#getting-started)). Keep any existing `\"`s. (If you use a different WiFi-enabled target, you may need to manually import its WiFi driver as described [here](https://github.com/ARMmbed/mbed-os-example-wifi#supported-hardware).)

1. For Ethernet (e.g on K64F), connect a cable to the port.

1. Connect a USB cable between the USB port on the board and the host computer.

1. <a name="build_cmd"></a> Run the following command to build the example project, program the microcontroller flash memory, and open a serial terminal:

    * Mbed CLI 2

    ```bash
    $ mbed-tools compile -m <TARGET> -t <TOOLCHAIN> --flash --sterm --baudrate=115200
    ```

    * Mbed CLI 1

    ```bash
    $ mbed compile -m <TARGET> -t <TOOLCHAIN> --flash --sterm --baudrate=115200
    ```

Alternatively, you can manually copy the binary to the board, which you mount on the host computer over USB.
The binary is located at:
* **Mbed CLI 2** - `./cmake_build/<TARGET>/<PROFILE>/<TOOLCHAIN>/mbed-os-example-for-aws.bin`</br>
* **Mbed CLI 1** - `./BUILD/<TARGET>/<TOOLCHAIN>/mbed-os-example-for-aws.bin`

Depending on the target, you can build the example project with the `GCC_ARM` or `ARM` toolchain. Run the command below to determine which toolchain supports your target:

```
$ mbed compile -S
```

### Device Shadow demo

Before running this demo, go to the AWS IoT Core console. In _Test_ -> _Subscribe to a topic_, subscribe to `$aws/things/<your thing>/shadow/#` (the `#` is a wildcard) on the AWS to monitor all Device Shadow updates. Then in _Publish to a topic_, publish to `$aws/things/<your thing>/shadow/update` the following payload:

```json
{
  "state": {
    "desired": {
      "DemoNumber": 200
    }
  }
}
```

Build with the same steps as the MQTT demo, but also set `aws-client.shadow` to `true` in [`mbed_app.json`](./mbed_app.json) before compilation.

## Expected output

### MQTT demo (default)

Once the example starts to run, you can [monitor a serial terminal](https://os.mbed.com/docs/mbed-os/v6.0/tutorials/serial-comm.html) to see that the device connects to your network, exchanges some TLS handshakes, connects to AWS and publishes to the topic you just subscribed. This can be seen on the AWS console as incoming messages.

The application publishes a message every second for 10 seconds, or until it receives a message from the cloud:
```
[INFO][Main]: Publishing "10 messages left to send, or until we receive a reply" to topic <your topic>
[INFO][Main]: Message sent successfully
[INFO][Main]: Publishing "9 messages left to send, or until we receive a reply" to topic <your topic>
[INFO][Main]: Message sent successfully
[INFO][Main]: Publishing "8 messages left to send, or until we receive a reply" to topic <your topic>
[INFO][Main]: Message sent successfully
```

You can send a message to your device via the AWS IoT Core console (_Test_ -> _Publish to a topic_). Use the same topic name you set in `aws-mqtt-topic.value` in [`mbed_app.json`](./mbed_app.json). Use the existing JSON structure and set "message" to one you want to send.

On receipt of a message, the application displays on the console the message you sent via the AWS IoT Core console.

### Device Shadow demo

The following will be printed to the serial:
```
[INFO][Main]: Device Shadow document downloaded
[INFO][Main]: Desired value of DemoNumber: <value set on the AWS console>
[INFO][Main]: Device Shadow reported string value published
[INFO][Main]: Device Shadow reported integer value published
```

## Troubleshooting
If you have problems, you can review the [documentation](https://os.mbed.com/docs/latest/tutorials/debugging.html) for suggestions on what could be wrong and how to fix it.

## Related Links

* [Mbed client for AWS IoT Core](https://github.com/ARMmbed/mbed-client-for-aws)
* [AWS IoT Core](https://aws.amazon.com/iot-core/)
* [Mbed OS Configuration](https://os.mbed.com/docs/latest/reference/configuration.html).
* [Mbed OS Serial Communication](https://os.mbed.com/docs/latest/tutorials/serial-communication.html).
* [Mbed boards](https://os.mbed.com/platforms/).

### License and contributions

The software is provided under Apache-2.0 license. Contributions to this project are accepted under the same license. Please see contributing.md for more info.

This project contains code from other projects. The original license text is included in those source files. They must comply with our license guide.
