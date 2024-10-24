# Build Stage
# First pull Golang image
FROM amd64/ubuntu:24.10 AS traderapp

#install tools 
RUN apt-get -y update && \
    apt-get -y install  \
        build-essential \
        cmake \
        python3 \
        libboost-dev \
        netcat-traditional \
        vim \
        gdb \
        cgdb \
	git

#copy application
WORKDIR /
COPY / ./
 
# Budild application makefiles
WORKDIR /tradeapp/build
RUN cmake .. 

#dev environment is ready and we can run the following to build and edit our code
#Now run cmake --build . --parallel 8
  
EXPOSE 1234/udp
CMD ["/bin/bash"]
