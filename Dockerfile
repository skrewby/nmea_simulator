FROM alpine:edge AS build

RUN apk add --no-cache cmake g++ make git linux-headers

WORKDIR /app
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build

FROM alpine:edge

RUN apk add --no-cache libstdc++

COPY --from=build /app/bin/nmea_simulator /usr/local/bin/

ENTRYPOINT ["nmea_simulator"]
