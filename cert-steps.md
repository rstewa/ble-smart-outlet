# Creating certs for SSL MQTT

## References

- http://www.steves-internet-guide.com/mosquitto-tls/
- http://www.steves-internet-guide.com/creating-and-using-client-certificates-with-mqtt-and-mosquitto/
- https://mosquitto.org/man/mosquitto-tls-7.html

## Create CA and server certs

1. Create a key pair for the CA

```bash
openssl genrsa -des3 -out ca.key 2048
```

2. Generate a certificate authority certificate and key.

```bash
openssl req -new -x509 -days 1826 -extensions v3_ca -keyout ca.key -out ca.crt
```

3. Generate a server key

```bash
openssl genrsa -aes256 -out server.key 2048
```

OR

Generate a server key without encryption.

```bash
openssl genrsa -out server.key 2048
```

4. Generate a certificate signing request to send to the CA.

```bash
openssl req -new -out server.csr -key server.key
```

5. Now we use the CA key to verify and sign the server certificate. This creates the server.crt file

```bash
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 360
```

7. Copy `ca.crt` to `/etc/mosquitto/ca_certificates` and copy `server.crt` and `server.key` to `/etc/mosquitto/certs`

8. Modify `/etc/mosquitto/mosquitto.conf`

```
user root
listener 8883

cafile /etc/mosquitto/ca_certificates/ca.crt
certfile /etc/mosquitto/certs/server.crt
keyfile /etc/mosquitto/certs/server.key

tls_version tlsv1.2

allow_anonymous true
require_certificate true
use_identity_as_username true
```

## Create client certs

1. Create a client private key

```bash
openssl genrsa -out client.key 2048
```

2. Next create a certificate request and use the client private key to sign it.

```bash
openssl req -new -out client.csr -key client.key
```

3. Now we complete the request and create a client certificate

```bash
openssl x509 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 360
```

4. Copy `ca.crt`, `client.crt` & `client.key` to your client