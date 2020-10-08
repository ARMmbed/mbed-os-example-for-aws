# Testing examples

Examples are tested using tool [htrun](https://github.com/ARMmbed/mbed-os-tools/tree/master/packages/mbed-host-tests) and templated print log.

To run the test, use following command after you build the example:
```
mbedhtrun -d D: -p COM4 -m K64F -f .\BUILD\K64F\GCC_ARM\mbed-os-example-for-aws.bin --compare-log tests\aws.log --baud-rate=115200 --sync=0
```


More details about `htrun` are [here](https://github.com/ARMmbed/mbed-os-tools/tree/master/packages/mbed-host-tests#testing-mbed-os-examples).
