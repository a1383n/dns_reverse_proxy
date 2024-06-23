# DNS Reverse Proxy

DNS Reverse Proxy is a C++ application designed to handle DNS queries by forwarding them to an external resolver and returning the responses. This project is useful for scenarios where you need to relay DNS queries through a proxy, possibly for logging, filtering, or modifying the questions/responses.

> [!NOTE]  
> **DNS Reverse Proxy is part of a larger microservices architecture. Explore [SecureProxy](https://github.com/a1383n/secure-proxy) for centralized management and administration, which provides an admin panel and central API for configuring and monitoring DNS and proxy servers.**

## Features

- Forwards DNS queries to an external resolver
- Returns DNS responses from the external resolver to the client
- Configurable using command-line flags
- JSON support for configuration and logging (Planed)
- Built with CMake for easy compilation and dependency management

## Dependencies

This project relies on the following libraries:

- **ldns**: A library for DNS programming
- **jsoncpp**: A library for handling JSON data in C++
- **gflags**: A library for command-line flag parsing
- **curl**: A library for transferring data with URLs

## Build Instructions

To build the DNS Reverse Proxy, follow these steps:

1. **Install Dependencies**: Ensure you have all the required libraries installed on your system.

   On Debian-based systems, you can install these using:
   ```sh
   sudo apt-get install build-essentials cmake libldns-dev libjsoncpp-dev libgflags-dev libcurl4-openssl-dev
   ```

2. **Clone the Repository**:
   ```sh
   git clone https://github.com/yourusername/dns-reverse-proxy.git
   cd dns-reverse-proxy
   ```

3. **Build the Project**:
   ```sh
   mkdir build
   cd build
   cmake ..
   cmake --build . --target dns_reverse_proxy
   ```
4. **Run**:
   ```sh
   ./dns-reverse-proxy
   ```

## Usage

The DNS Reverse Proxy can be configured using command-line flags. Here are some of the available options:

- `--endpoint`: Specify the external API to resolve the IP address
- `--port`: Specify the port that the DNS server should listen on
- `--debug`: Enable debug mode

Example:
```sh
./dns-reverse-proxy --endpoint http://127.0.0.1:8000 --port 53
```
### To-Do:
- [ ] Improve error handling and optimize for serving high amounts of traffic
- [ ] Implement DoH (DNS over HTTPS), DoT (DNS over TLS), DoQ (DNS over QUIC), and DNSCrypt for secure communication
- [ ] Add memory caching for faster response times
- [ ] Add JSON support for configuration and logging
- [ ] Implement rate limiting to prevent abuse
- [ ] Enhance logging and monitoring for better insights
- [ ] Support IPv6
- [ ] Implement load balancing for better distribution of traffic
- [ ] Optimize DNS query performance
- [ ] Implement a robust access control and authentication system
- [ ] Improve documentation and provide comprehensive usage guides

## Contributing

Contributions are welcome! Please submit a pull request or open an issue to discuss any changes or enhancements.

## License

This project is licensed under the GPL-3 License. See the [LICENSE](LICENSE) file for details.
