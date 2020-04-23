# Configuration
Set the Certificates & Key in `aws_certificates.c` and `AWS_IOT_MQTT_HOST` in `aws_iot_config.h`

# Policy

```json
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "iot:*",
      "Resource": "arn:aws:iot:your-region:your-account-id:*"
    }
  ]
}
```

Remember to change `your-region` and `your-account-id`
