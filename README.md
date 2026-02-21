# NMEA Simulator

An application used to test NMEA2000 devices.

Initially developed to test the Actisense NGX-1-ISO gateway behaviour. The CAN and Serial sides were both connected to the PC and the program was run

```sh
./bin/nmea_simulator -c can0 -s /dev/ttyUSB0 -b 4800 -C config/example_config.toml
```

## Running through Docker

Build the image

```sh
docker buildx build -t nmea_simulator .
```

Run

```sh
docker run --network host --device /dev/ttyUSB0 nmea_simulator -C config.toml -c can0 -s /dev/ttyUSB0 -b 4800
```
