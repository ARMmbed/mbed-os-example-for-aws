![](./resources/official_armmbed_example_badge.png)

# AWS Mbed OS example

The example project is part of the [Arm Mbed OS Official Examples](https://os.mbed.com/code/). It contains an application that connects to an AWS MQTT broker and publishes a message every 1 second for 10 seconds or until a message is received.

You can build the project with all supported [Mbed OS build tools](https://os.mbed.com/docs/mbed-os/latest/tools/index.html). However, this example project specifically refers to the command-line interface tool [Arm Mbed CLI](https://github.com/ARMmbed/mbed-cli#installing-mbed-cli).
(Note: To see a rendered example you can import into the Arm Online Compiler, please see our [import quick start](https://os.mbed.com/docs/mbed-os/latest/quick-start/online-with-the-online-compiler.html#importing-the-code).)

## Downloading this project
1. [Install Mbed CLI](https://os.mbed.com/docs/mbed-os/latest/quick-start/offline-with-mbed-cli.html).

1. Clone this repository on your system, and change the current directory to where the project was cloned:

    ```
    $ git clone git@github.com:armmbed/mbed-os-example-aws && cd mbed-os-example-aws
    $ mbed deploy
    ```

    Alternatively, you can download the example project with Arm Mbed CLI using the `import` subcommand:

    ```
    $ mbed import mbed-os-example-aws && cd mbed-os-example-aws
    ```


## Configuring the AWS IoT Core service

1. Create an AWS account if you don't have one, and login to it.

    __NOTE:__ If you have an admin for your AWS account, please contact them to add a user to the account. You should obtain your login credentials from your admin in this case.

1. Set up device credentials and policy via the AWS IoT console. You can refer to the AWS documentation [here](https://docs.aws.amazon.com/iot/latest/developerguide/iot-gs.html). Follow the steps there to

    * create a thing
    * generate a device certificate and keys
    * create an IoT policy and attach that policy to your device.

    Also download "Amazon Root CA 1" from [here](https://docs.aws.amazon.com/iot/latest/developerguide/server-authentication.html#server-authentication-certs).

    Once you have downloaded the credentials, you will need to place them in the [`aws_credentials.h`](./aws_credentials.h) file of this example.

1. Once you have created the "thing", you will need to obtain the custom endpoint name from the console. At the time of writing this document, you can find it under "Settings" in the IoT console.

    In [`mbed_app.json`](./mbed_app.json) file, set `aws-endpoint.value` to be that of the custom endpoint.

1. Set a topic that both your device and the cloud can publish messages to.

   In [`mbed_app.json`](./mbed_app.json) file, set `aws-mqtt-topic.value` to a name you prefer, or use the default one. On the AWS console, you will then need to subscribe to the same topic name. At the time of writing this document, you can find this under "Test" on the console.

1. Give your device a name by setting `aws-client-identifier` in [`mbed_app.json`](./mbed_app.json).

## Building and running

1. If using WiFi (e.g. on DISCO_L475VG_IOT01A), enter your network's SSID and password in [`mbed_app.json`](./mbed_app.json) (see [here](https://github.com/ARMmbed/mbed-os-example-wifi/blob/master/README.md#getting-started)). Keep any existing `\"`s. (If you use a different WiFi-enabled target, you may need to manually import its WiFi driver as described [here](https://github.com/ARMmbed/mbed-os-example-wifi#supported-hardware).)

1. For Ethernet (e.g on K64F), connect a cable to the port.

1. Connect a USB cable between the USB port on the board and the host computer.

1. <a name="build_cmd"></a> Run the following command to build the example project, program the microcontroller flash memory, and open a serial terminal:
    ```
    $ mbed compile -m detect -t <TOOLCHAIN> --flash --sterm
    ```

Alternatively, you can manually copy the binary to the board, which you mount on the host computer over USB.
The binary is located at `./BUILD/<TARGET>/<TOOLCHAIN>/mbed-os-example-aws.bin`.

Depending on the target, you can build the example project with the `GCC_ARM` or `ARM` toolchain. Run the command below to determine which toolchain supports your target:

```
$ mbed compile -S
```

## Expected output

Once the example starts to run, you can [monitor a serial terminal](https://os.mbed.com/docs/mbed-os/v6.0/tutorials/serial-comm.html) to see that the device connects to your network, exchanges some TLS handshakes, connects to AWS and publishes to the topic you just subscribed. This can be seen on the AWS console as incoming messages.

The application publishes a message every second for 10 seconds, or until it receives a message from the cloud:
```
[INFO][Main]: sending warning message: Warning: Only 10 second(s) left to say your name !
[INFO][Main]: sending warning message: Warning: Only 9 second(s) left to say your name !
[INFO][Main]: sending warning message: Warning: Only 8 second(s) left to say your name !
[INFO][Main]: sending warning message: Warning: Only 7 second(s) left to say your name !
```

You can send a message to your device via the AWS IoT console (Test -> Publish to a topic). Use the same topic name you set in `aws-mqtt-topic.value` in [`mbed_app.json`](./mbed_app.json).

On receipt of a message, the application displays on the console the message prefixed with `Hello␣`.

## Troubleshooting
If you have problems, you can review the [documentation](https://os.mbed.com/docs/latest/tutorials/debugging.html) for suggestions on what could be wrong and how to fix it.

## Related Links

* [Mbed OS Stats API](https://os.mbed.com/docs/latest/apis/mbed-statistics.html).
* [Mbed OS Configuration](https://os.mbed.com/docs/latest/reference/configuration.html).
* [Mbed OS Serial Communication](https://os.mbed.com/docs/latest/tutorials/serial-communication.html).
* [Mbed OS bare metal](https://os.mbed.com/docs/mbed-os/latest/reference/mbed-os-bare-metal.html).
* [Mbed boards](https://os.mbed.com/platforms/).
* [AWS IoT Core](https://aws.amazon.com/fr/iot-core/)
* [AWS IoT Core - Embedded C SDK](https://github.com/aws/aws-iot-device-sdk-embedded-C/tree/v4_beta)

### License and contributions

The software is provided under Apache-2.0 license. Contributions to this project are accepted under the same license. Please see contributing.md for more info.

This project contains code from other projects. The original license text is included in those source files. They must comply with our license guide.
