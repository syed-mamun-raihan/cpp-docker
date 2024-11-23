# Build Stage
# First pull Golang image
FROM amd64/ubuntu:24.10 AS apps

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
        cpplint \
        clang-format \
        pre-commit \
        doxygen \
	    git \
        catch2 \
        openjdk-21-jdk \
        golang-go

#copy application
WORKDIR /
COPY / ./
 
# Budild application makefiles
WORKDIR /apps/build
RUN cmake .. 

#dev environment is ready and we can run the following to build and edit our code
#Now run cmake --build . --parallel 8
  
EXPOSE 1234/udp
CMD ["/bin/bash"]
