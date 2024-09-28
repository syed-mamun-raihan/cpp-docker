# Build Stage
# First pull Golang image
FROM amd64/ubuntu:24.04 AS traderapp

#install tools 
RUN apt-get -y update && \
    apt-get -y install  \
        build-essential \
        cmake \
        python3 \
        libboost-dev \
        netcat-traditional \
        vim

#copy application
WORKDIR /myapp
COPY /myapp/src/ ./src/
COPY /myapp/CMakeLists.txt .
 
# Budild application makefiles
WORKDIR /myapp/build
RUN cmake .. 

#Now run cmake --build . --parallel 8
  
EXPOSE 1234/udp
CMD ["/bin/bash"]
