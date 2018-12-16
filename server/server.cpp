/**
 * CSE 411
 * Fall 2018
 * Programming Assignment #5 Part 2
 *
 * It is exceedingly rare for any real-world software to be written in a single
 * language, or to be completely self contained.  Modern software relies on
 * services provided by other software packages, and not just by way of linking
 * in libraries... modern software also tends to request services from on-line
 * servers, who expose Application Programming Interfaces (APIs) that govern how
 * they can be used.
 *
 * In this assignment, we will build a multi-language, multi-component program
 * that consumes a remote service.  There will be two C++ components, and an
 * HTML5 component.  These components will work together as a Twitter analytics
 * package.
 *
 * In more detail: at any time, the client program can be run.  It will take a
 * command line parameter, which is a search term, and it will issue a query
 * against the Twitter API.  It will process the result set as follows:
 *     Produce a histogram from the top 100 results, matching number of tweets
 *     to dates.  Save this in a MongoDB collection called "hist".  The query
 *     term should be the document name.
 * - Note: query terms that are user handles (@name) or hashtags (#tag) will
 *   naturally show user activity or topic trends
 * - Note: authentication for the Twitter API uses OAuth.  It will take some
 *   time for you to figure it out.  Plan accordingly.
 *
 * The server program should do three things.
 *   - It should serve an index.html file in response to GET /
 *   - It should serve other files in response to GET /files/<filename>
 *   - It should serve JSON, pulled from MongoDB, in response to GET /data/term
 * With regard to the third of these tasks, "term" should be a search term, and
 * the response should be a JSON representation of the histogram for that term,
 * as determined by the client.  If no histogram exists, then the JSON response
 * should be empty.
 *
 * The HTML5 component should be a single-page web app that allows the user to
 * enter a search term.  In response to that term, the page will issue an AJAX
 * request to the server to get some JSON data, and then it should use that JSON
 * data to initialize a chart.  You can use an existing JavaScript charting
 * library, such as Dygraphs, Chart.js, D3.js, etc.  You may also use other
 * JavaScript libraries (jQuery, Bootstrap, etc.), as you see fit.
 *
 * Your program should use Microsoft's cpprestsdk.  It should use advanced
 * features of C++ whenever appropriate.  It should compile with g++ version 7.
 * You should organize your code into files in a way that is appropriate.  You
 * should comment your code.  You should only use C++ libraries that are easily
 * installed into an Ubuntu Docker container.
 *
 *Turn-in will be via bitbucket.
 */

// #include <iostream>

// #include "include/stdafx.h"
// #include "include/handler.h"

// #include <cstdint>
// #include <mongocxx/instance.hpp>
// #include <bsoncxx/json.hpp>
// #include <mongocxx/client.hpp>
// #include <mongocxx/stdx.hpp>
// #include <mongocxx/uri.hpp>

// #include <cpprest/http_client.h>
// #include <cpprest/filestream.h>

// using namespace utility;                    // Common utilities like string conversions
// using namespace web;                        // Common features like URIs.
// using namespace web::http;                  // Common HTTP functionality
// using namespace web::http::client;          // HTTP client features
// using namespace concurrency::streams;  
// using namespace pplx;

// using namespace std;
// using namespace http;
// using namespace http::experimental::listener;

// std::unique_ptr<handler> g_httpHandler;


// /** Send file in response to a 'GET /' request */
// void handle_get(web::http::http_request request) { 
// // Get the resource path    
//     web::uri res = request.relative_uri();
//     std::string res_str = res.to_string();
//     // If the browser is requesting a file  
//     if (res_str == "/") 
//     { // If the request is "/", serve index.html        
//     std::string filename = "index.html";        
//     std::ifstream file(filename);       
//     std::stringstream buffer;       
//     buffer << file.rdbuf();     

//     // Serve the requested file     
//     request.reply(web::http::status_codes::OK, buffer.str(), "text/html");  
//     }

//     else if (res_str.find("/files/") == 0) {      
//     // Get the name of the requested file       
//         std::string filename = res_str.substr(7);       
//         std::ifstream file(filename);       
//         std::stringstream buffer;       
//         buffer << file.rdbuf();     

//         // Serve the requested file     
//         request.reply(web::http::status_codes::OK, buffer.str(), "text/html");  
//     }

//     // Get the search term  
//     // http://localhost:34568/data/data/eashan
//     // /data/eashan
//     if (res_str.find("/data") == 0) {       
//         std::string term = res_str.substr(6);       
//         std::cout << res_str << std::endl; 

//         // bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
//         // collection.find_one(document{} << "i" << res_str << finalize);
//         // if(maybe_result) {
//         // std::cout << bsoncxx::to_json(*maybe_result) << "\n";
//         // }

//     }
// }



// void on_initialize(const string_t& address)
// {


//     uri_builder uri(address);
  

//     auto addr = uri.to_uri().to_string();
//      g_httpHandler = std::unique_ptr<handler>(new handler(addr));
//      g_httpHandler->open().wait();

//     ucout << utility::string_t(U("Listening for requests at: ")) << addr << std::endl;

//     return;
// }

// void on_shutdown()
// {
//      g_httpHandler->close().wait();
//     return;
// }

// #ifdef _WIN32
// int wmain(int argc, wchar_t *argv[])
// #else

// int main(int argc, char *argv[])
// #endif
// {
//     utility::string_t port = U("34568");
//     if(argc == 2)
//     {
//         port = argv[1];
//     }

//     utility::string_t address = U("http://172.17.0.3:");
//     address.append(port);

//     web::uri_builder uri(address);
//     uri.append_path(U("data"));

//     std::string addr = uri.to_uri().to_string();
  
//     web::http::experimental::listener::http_listener listener(addr);

//     listener.support(web::http::methods::GET, handle_get);

//     listener
//     .open()
//     .then([]() {std::cout << "Listening on http://localhost:34568/data/..\n";})
//     .wait();

//     on_initialize(address);
//     std::cout << "Press ENTER to exit." << std::endl;

//     std::string line;
//     std::getline(std::cin, line);

//     listener.close().wait();

//     on_shutdown();
//     return 0;
// }


#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <fstream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/http_listener.h>              // HTTP server
#include <cpprest/json.h>                       // JSON library
#include <cpprest/uri.h>                        // URI library
#include <cpprest/ws_client.h>                  // WebSocket client
#include <cpprest/containerstream.h>            // Async streams backed by STL containers
#include <cpprest/interopstream.h>              // Bridges for integrating Async streams with STL and WinRT streams
#include <cpprest/rawptrstream.h>               // Async streams backed by raw pointer to memory
#include <cpprest/producerconsumerstream.h>     // Async streams for producer consumer scenarios

#include <cstdint>
#include <vector>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

using namespace mongocxx;
using namespace bsoncxx;


using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
using namespace web::http::experimental::listener;          // HTTP server
using namespace web::experimental::web_sockets::client;     // WebSockets client
using namespace web::json; 

using bsoncxx::builder::stream::finalize;                                 // JSON library

using namespace std;

// /** Send file in response to a 'GET /' request */
void handle_get(web::http::http_request request) { 
// Get the resource path    
    web::uri res = request.relative_uri();
    std::string res_str = res.to_string();
    // If the browser is requesting a file  
    if (res_str == "/") 
    { // If the request is "/", serve index.html        
    std::string filename = "index.html";        
    std::ifstream file(filename);       
    std::stringstream buffer;       
    buffer << file.rdbuf();     

    // Serve the requested file     
    request.reply(web::http::status_codes::OK, buffer.str(), "text/html");  
    }

    else if (res_str.find("/file/") == 0) {      
    // Get the name of the requested file       
        std::string filename = res_str.substr(7);       
        std::ifstream file(filename);       
        std::stringstream buffer;       
        buffer << file.rdbuf();     

        // Serve the requested file
        std::cout<<"Serving the requested file."<<std::endl;   
        request.reply(web::http::status_codes::OK, buffer.str(), "text/html");  
    }

    // Get the search term  
    // http://localhost:34568/data/eashan
    // /data/eashan
    if (res_str.find("/data") == 0) {       
        std::string term = res_str.substr(6);       
        std::cout << "Term: " << term << std::endl; 

        mongocxx::instance inst{};
        std::string username = "eashan22";
        std::string pass = "horwEh-2zexmi-gyjvix";
        mongocxx::client conn {
        mongocxx::uri{"mongodb://"+username+":"+pass+"@ds042607.mlab.com:42607/project5"} };
        auto collection = conn["project5"]["Twitter_collection"]; //[Database][collection]

        // bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
        // collection.find_one(bsoncxx::builder::stream::document{} << "Key" << term << finalize);
        // if(maybe_result){
        //   std::cout << bsoncxx::to_json(*maybe_result)<<"\n";
        //   std::cout << "Found the JSON data and passing it as request reply." << std::endl;
        //   request.reply(web::http::status_codes::OK, bsoncxx::to_json(*maybe_result), "text/javascript");
        // } else {
        //   std::cout << "Failed to get mongo data" << std::endl;
        //   request.reply(web::http::status_codes::NotFound, "{status: \"failed\"}", "text/javascript");
        // }
        mongocxx::cursor cursor = collection.find(bsoncxx::builder::stream::document{} << "Key" << term << finalize);
            for(auto doc : cursor) {
              std::cout << bsoncxx::to_json(doc) << "\n";
              request.reply(web::http::status_codes::OK, bsoncxx::to_json(doc), "text/javascript");
            }
    }
}

int main(int argc, char *argv[]) {
  // By now, you should be able to write your own argument parsing code :)
  utility::string_t address = U("http://172.17.0.4:");
  address.append(U("34568"));
  web::uri_builder uri(address);
  //uri.append_path(U("data"));

  std::string addr = uri.to_uri().to_string();
  web::http::experimental::listener::http_listener listener(addr);
  listener.support(web::http::methods::GET, handle_get);

  listener
  .open()
  .then([]() {std::cout << "Listening on http://localhost:34568/\n";})
  .wait();

  std::string line;
  std::getline(std::cin, line);

  listener.close().wait();
}
