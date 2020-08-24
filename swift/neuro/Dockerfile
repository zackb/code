FROM swift:latest as builder
WORKDIR /root
COPY . .
RUN make

FROM swift:slim
WORKDIR /root
COPY --from=builder /root/.build/x86_64-unknown-linux-gnu/release/neuro /root/neuro
CMD ["/root/neuro"]
