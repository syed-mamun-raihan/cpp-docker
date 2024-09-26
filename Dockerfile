# Build Stage
# First pull Golang image
FROM amd64/ubuntu:24.10 as build-docker

#install tools 
RUN apt-get update && \
    apt-get install  \
        build-essential \
        cmake \
        python3 \
        libboost-dev \
        netcat-traditional \

#copy application
WORKDIR ./workspace
COPY src/ ./src/
COPY CMakeLists.txt .
 
# Budild application
WORKDIR ./workspace/build

RUN cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . --parallel 8
  
# Start app
ENTRYPOINT [ "./workspace/build/src/myapp" ]
