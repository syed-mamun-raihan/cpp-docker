# Kraken-Task

Project Structure:
    /Dockerfile
    /compose.yaml
    /README.md
    /docker_build.sh
    /docker_shell.sh
    /myapp/CMakeLists.txt
    /myapp/src/TraderApp.cpp
    /myapp/src/TradeProcessor.cpp
    /myapp/src/include/TradeProcessor.h
    /myapp/build

Design:
    The main udp server TradeApp has io_service loop to handle all inputs.
    It employes worker Thread for outputs.
    It uses mutexes to push input requests.

    Output threads pick input requests and parses and then process using TradeProcessor.
    It uses mutexes to synchronize operations in the output threads

    Currently it has 1 input threads and 1 output threads. 

Dev environment:    
    1. Run docker_build.sh
    2. Check instance id from Docker Desktop (Docker desktop need tobe installed or docker cli maybe used also)
    3. Run docker_shell.sh <imageid>

    This will give access to dev environment.

Run:
    1. Run <cmake --build .> from build folder
    2. Output is /myapp/build/trader
    3. Run /myapp/build/trader > output.csv

Todo:
    1. Consider abstraction of order execution
    2. Add doxygen document generation
    3. Add gtest or similar for unit testing
    4. use ‘-‘ for side elimination - not implemented yet. Need to be implemented
    5. Performance testing is needed

