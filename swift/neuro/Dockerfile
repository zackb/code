FROM swift:latest as builder
WORKDIR /root
COPY . .
run echo "deb [trusted=yes] http://apt.orkhanalikhanov.com ./" | tee -a /etc/apt/sources.list
run apt-get update
run apt-get install libressl
RUN make linux

FROM swift:slim
WORKDIR /root
COPY --from=builder /root .build/x86_64-unknown-linux/release
CMD ["neuro"]
