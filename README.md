# Hungrynerds Web Server
## Source Code Layout
Here is the directory tree of the source code:
```bash
├── cmake
├── config
├── docker
├── expect
├── include
│   ├── *.h
├── src
│   ├── *.cc
├── tests
│   ├── staticTest
│   ├── *_test.cc
├── .dockerignore
├── .gitignore
├── build.sh
├── CMakeLists.txt
├── integration.sh
└── README.md
```
All C++ header files are located in `include` folder and their corresponding implementation and test files are located in `src` folder and `tests` folder respectively.
## APIs Usage
Before starting the server, you need to parse the configuration file and save it as a `NginxConfig` object, which will be passed (by pointer or reference) to many other classes, e.g., session, handler manager. For example, you can parse the configuration file and start the server in this way:
```C++
boost::asio::io_service io_service;
NginxConfig config;
NginxConfigParser config_parser;
if (!config_parser.Parse("configuration file path", &config)) {
    throw std::runtime_error("error message");
}
server s(io_service, &config);
io_service.run();
```
The next step is parsing the HTTP request. We cited Alex Nekipelov's [HTTP parser library](https://github.com/nekipelov/httpparser) for this task. There are three states: ParsingIncompleted, ParsingError, and ParsingCompleted. The example usage likes this:
```C++
Request req;
httpparser::HttpRequestParser::ParseResult result = dataProcessor.parseHttpRequest(&req);
if (result == httpparser::HttpRequestParser::ParsingIncompleted) {
    // TODO: Handle parsing incompleted
} else if (result == httpparser::HttpRequestParser::ParsingError) {
    // TODO: Handle parsing error
} else {
    // TODO: Handle parsing completed
}
```
If the HTTP request is valid, then you need to parse the URL for the path that the client requested. Again, we cited Alex Nekipelov's [URL parsing library](https://github.com/nekipelov/httpparser) and implemented `UrlProcessor` for this task. The example code likes this:
```C++
Request req;
std::string host;
if(dataProcessor.check_request(&req, &host)) {
    req.uri = dataProcessor.construct_url(&req, host);
}
urlProcessor.setup(server_config);
auto status = urlProcessor.handle_url(req);
if (status) {
    if (urlProcessor.give_RequestContent() == UrlProcessor::ECHO_) {
        // TODO: Handle the echo path
    } else if (urlProcessor.give_RequestContent() == UrlProcessor::STATIC_) {
        // TODO: Handle the static path
    } else if (urlProcessor.give_RequestContent() == UrlProcessor::STATUS_) {
        // TODO: Handle the status path
    } else {
        // TODO: Handle the invalid (404) path
    }
}
```
Finally, you should construct an appropriate handler to give a response. There are four handlers (echo, invalid, static, status), which inherit the common handler that is managed by the handler manager. You should initialize a handler object by using the handler manager only. You can get a handler pointer by calling the `createByName` method in the common handler (polymorphism), and use that pointer to give an appropriate response by calling `HandleRequest`. Let's say you want to handle a request for the echo path in a session, you can create an echo handle in this way:
```C++
HandlerManager handler_manager;
std::unique_ptr<reply> res = handler_manager.createByName("ECHO", *server_config, server_config->root_path)->HandleRequest(req);
```
For detailed usage and implementation, please refer to `session.cc`, `UrlProcessor.cc`, and other related files.
## Build, Test, Run
Since we are testing the web server in a real browser, you should start the Docker environment with port mapping on. The command likes this:
```bash
$ tools/env/start.sh [args] -- -p 127.0.0.1:8080:8080
```
To compile the code, you can run:
```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```
To test the code, you can run:
```bash
$ make test
```
To generate the code coverage report, you can run:
```bash
$ mkdir build_coverage
$ cd build_coverage
$ cmake -DCMAKE_BUILD_TYPE=Coverage ..
$ make coverage
```
At this point, you can create a configuration file for the web server. The sample configuration file likes this:
```
listen   8080;

root      /www;

handler static {
  location      /static1;
  root        /static;
}

handler echo {
  location      /echo;
}

handler status {
  location      /status;
}
```
Suppose we save the configuration file as `config` and put this file inside `config` folder, then you can start the web server using this command:
```bash
$ bin/server ../config/config
```

Let's say you have an `index.html` file in `/www/static` directory, then you can visit this file using this URL: `localhost:8080/static1/index.html`. You can visit the `echo` and `status` path by using URLs: `localhost:8080/echo` and `localhost:8080/status` respectively.