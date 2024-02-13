FROM alpine:3.17.0 as prereq
RUN apk add --no-cache libstdc++ libgcc

FROM prereq as build

RUN apk add --no-cache build-base git g++ cmake openssl-dev openssl-libs-static zlib-dev zlib-static
RUN git clone https://github.com/brainboxdotcc/DPP -b v10.0.29 --depth 1 --single-branch;
RUN cd DPP; cmake -B ./build -DBUILD_SHARED_LIBS=OFF -DDPP_BUILD_TEST=OFF; cmake --build ./build -j$(nproc);
RUN cd DPP/build; make install
WORKDIR /app
COPY . .
RUN cmake -B ./build -DCMAKE_BUILD_TYPE=Release -DUSING_DOCKER=TRUE -DBUILD_SHARED_LIBS=OFF
RUN cmake --build ./build

FROM prereq as run
COPY --from=build /app/build/maiauth .
RUN chmod +x ./maiauth
CMD ./maiauth