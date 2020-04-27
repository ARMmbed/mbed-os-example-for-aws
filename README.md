![](./resources/official_armmbed_example_badge.png)

# AWS Mbed OS example

The example project is part of the [Arm Mbed OS Official Examples](https://os.mbed.com/code/). It contains an application that connects to an AWS MQTT broker and publishes a message every 1 second for 10 seconds or until a message is received.

You can build the project with all supported [Mbed OS build tools](https://os.mbed.com/docs/mbed-os/latest/tools/index.html). However, this example project specifically refers to the command-line interface tool [Arm Mbed CLI](https://github.com/ARMmbed/mbed-cli#installing-mbed-cli).
(Note: To see a rendered example you can import into the Arm Online Compiler, please see our [import quick start](https://os.mbed.com/docs/mbed-os/latest/quick-start/online-with-the-online-compiler.html#importing-the-code).)

1. [Install Mbed CLI](https://os.mbed.com/docs/mbed-os/latest/quick-start/offline-with-mbed-cli.html).

1. Clone this repository on your system, and change the current directory to where the project was cloned:

    ```bash
    $ git clone git@github.com:armmbed/mbed-os-example-aws && cd mbed-os-example-aws
    $ mbed deploy
    ```

    Alternatively, you can download the example project with Arm Mbed CLI using the `import` subcommand:

    ```bash
    $ mbed import mbed-os-example-aws && cd mbed-os-example-aws
    ```

## Building and running

1. Configure the example in `mbed_app.json` and add your credentials in `aws_credentials.h`.
1. Connect a USB cable between the USB port on the board and the host computer.
1. <a name="build_cmd"></a> Run the following command to build the example project and program the microcontroller flash memory:
    ```bash
    $ mbed compile -m detect -t <TOOLCHAIN> --flash
    ```

Alternatively, you can manually copy the binary to the board, which you mount on the host computer over USB.
The binary is located at `./BUILD/<TARGET>/<TOOLCHAIN>/mbed-os-example-aws.bin`.

Depending on the target, you can build the example project with the `GCC_ARM`, `ARM` or `IAR` toolchain. After installing Arm Mbed CLI, run the command below to determine which toolchain supports your target:

```bash
$ mbed compile -S
```

## Expected output

The Application connects to an AWS MQTT broker and publishes a message every 1 second for 10 seconds or until a message is received.
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
