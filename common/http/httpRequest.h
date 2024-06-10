#include <sstream>
#include <map>
class HttpRequest {
    private:
        std::string method;
        std::string url;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;
        

    public:
        std::string response;
        void parse_request(const std::string& request) {
        }

        std::string generate_response() {
            // Generate the HTTP response based on the method, url, and version
            // This is just a simple example, you should generate a proper HTTP response here
            return "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
        }
        void setParams(std::string method, std::string url, std::string version, std::map<std::string, std::string> headers, std::string body){
            this->method = method;
            this->url = url;
            this->version = version;
            this->headers = headers;
            this->body = body;
        }
};