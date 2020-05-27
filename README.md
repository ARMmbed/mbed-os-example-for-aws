![](./resources/official_armmbed_example_badge.png)

# AWS Mbed OS example

The example project is part of the [Arm Mbed OS Official Examples](https://os.mbed.com/code/). It contains an application that connects to an AWS MQTT broker and publishes a message every 1 second for 10 seconds or until a message is received.

You can build the project with all supported [Mbed OS build tools](https://os.mbed.com/docs/mbed-os/latest/tools/index.html). However, this example project specifically refers to the command-line interface tool [Arm Mbed CLI](https://github.com/ARMmbed/mbed-cli#installing-mbed-cli).
(Note: To see a rendered example you can import into the Arm Online Compiler, please see our [import quick start](https://os.mbed.com/docs/mbed-os/latest/quick-start/online-with-the-online-compiler.html#importing-the-code).)

## Summary of steps:
1. Obtain the necessary keys and certificates to connect a device to AWS. This can be done in various ways after logging in to the AWS IoT console. You can refer to AWS documentation [here](https://docs.aws.amazon.com/iot/latest/developerguide/iot-gs.html). Follow the steps listed on AWS documentation to create an IoT policy for your device and attaching that policy to your device.   
   __NOTE:__ If you have an admin for your AWS account, please contact them to add a user to the account. You should obtain your login credentials from your admin in this case.
1. Once you have downloaded the credentials, you will need to place them in the [`aws_credentials.h`](./aws_credentials.h) file.
1. Once you have created the "thing", you will need to obtain the custom endpoint name from the console. At the time of writing this document, you can find it under "Settings" in the IoT console. In [`mbed_app.json`](./mbed_app.json) file, set `aws-endpoint.value` to be that of the custom endpoint.
1. If using WiFi, you will also need to enter your network's SSID and password in [`mbed_app.json`](./mbed_app.json) (see [here](https://github.com/ARMmbed/mbed-os-example-wifi/blob/master/README.md#getting-started)).
1. You can then build the example and flash your target with the newly built binary using Arm Mbed CLI.
1. On the AWS console, you will then need to subscribe and publish to the topic created. At the time of writing this document, you can find this under "Test" on the console.
1. With the example running, you can [monitor a serial terminal](https://os.mbed.com/docs/mbed-os/v6.0/tutorials/serial-comm.html) to see that the device connects to your network, exchanges some TLS handshakes, connects to AWS and publishes to the topic you just subscribed. This can be seen on the AWS console as incoming messages.
1. You can also publish to this topic from the AWS console. This example demonstrates a string being appended as `"Hello <your string>!"`. You can "send" this from the AWS console and see the device responding with the `received + appended` string on the serial terminal of your device.

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

## Building and running

1. Configure the example using [`mbed_app.json`](./mbed_app.json) and add your credentials in [`aws_credentials.h`](./aws_credentials.h).
1. Connect a USB cable between the USB port on the board and the host computer.
1. <a name="build_cmd"></a> Run the following command to build the example project, program the microcontroller flash memory, and open a serial terminal:
    ```
    $ mbed compile -m detect -t <TOOLCHAIN> --flash --sterm
    ```

Alternatively, you can manually copy the binary to the board, which you mount on the host computer over USB.
The binary is located at `./BUILD/<TARGET>/<TOOLCHAIN>/mbed-os-example-aws.bin`.

Depending on the target, you can build the example project with the `GCC_ARM` or `ARM` toolchain. After installing Arm Mbed CLI, run the command below to determine which toolchain supports your target:

```
$ mbed compile -S
```

## Expected output

The application connects to an AWS MQTT broker and publishes a message every second for 10 seconds or until a message is received.
On reception of a message, the application displays on the console the message prefixed with `Hello␣`.


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
