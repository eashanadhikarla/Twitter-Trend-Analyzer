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

#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <fstream>
#include <cstdint>
#include <vector>
#include <dirent.h>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/http_listener.h>             // HTTP server
#include <cpprest/json.h>                       // JSON library
#include <cpprest/uri.h>                        // URI library
#include <cpprest/ws_client.h>                  // WebSocket client
#include <cpprest/containerstream.h>            // Async streams backed by STL containers
#include <cpprest/interopstream.h>              // Bridges for integrating Async streams with STL and WinRT streams
#include <cpprest/rawptrstream.h>               // Async streams backed by raw pointer to memory
#include <cpprest/producerconsumerstream.h>     // Async streams for producer consumer scenarios

#include <mongocxx/instance.hpp>
#include <mongocxx/cursor.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/stream/document.hpp>

using namespace mongocxx;
using namespace bsoncxx;
using namespace utility;                                    // Common utilities like string conversions
using namespace web;                                        // Common features like URIs.
using namespace web::http;                                  // Common HTTP functionality
using namespace web::http::client;                          // HTTP client features
using namespace concurrency::streams;                       // Asynchronous streams
using namespace web::http::experimental::listener;          // HTTP server
using namespace web::experimental::web_sockets::client;     // WebSockets client
using namespace web::json; 

using bsoncxx::builder::stream::finalize;
using namespace std;

// Query from MongoDB
web::json::value query(std::string term, auto collection){
  std::string out, res;
  web::json::value root_cpp, temp_cpp;
  std::ofstream fout;
  int i=0;

  mongocxx::cursor cursor = collection.find(
    bsoncxx::builder::stream::document{} << "Key" << term << bsoncxx::builder::stream::finalize);
  for(auto doc : cursor) {
    res = bsoncxx::to_json(doc);
    temp_cpp = web::json::value::parse(res);
    root_cpp["term"] = temp_cpp["term"];
    root_cpp["histogram"][i]["date"] = temp_cpp["date"];
    root_cpp["histogram"][i++]["num"] = temp_cpp["num"];
  }
  root_cpp["num_date"] = i;
  
  if (root_cpp["num_date"] != 0){
    fout.open(term + ".json");
    fout << root_cpp;
    fout.close();
  }
  return root_cpp;
}

// /** Send file in response to a 'GET /' request */
void handle_get(web::http::http_request request) { 
	// Get the resource path    
    web::uri res = request.relative_uri();
    std::string res_str = res.to_string();
    web::json::value root, list_term;
    vector<std::string> files;

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

        //******************************************************************************
        mongocxx::instance inst{};
		std::string username = "eashan22";
		std::string pass = "horwEh-2zexmi-gyjvix";
		mongocxx::client conn {
		mongocxx::uri{"mongodb://"+username+":"+pass+"@ds042607.mlab.com:42607/project5"} };
		auto collection = conn["project5"]["Twitter_collection"]; //[Database][collection]
        
        root = query(term, collection);

        //******************************************************************************

        struct dirent *ptr;
        vector<std::string> files;
        DIR *dir;
        std::string PATH = "./";
        std::string temp_json;
        dir = opendir(PATH.c_str());
        while((ptr=readdir(dir))!=NULL){
          if(ptr->d_name[0] == '.')
            continue;
          if ( (temp_json = ptr->d_name).find(".json") != std::string::npos){
            files.push_back(ptr->d_name);
          }
        }
        
        for (int i = 0; i < files.size(); ++i){
          root["json"][i] = web::json::value(files[i]);
        }

        closedir(dir);
        request.reply(web::http::status_codes::OK, root);

        // mongocxx::instance inst{};
        // std::string username = "eashan22";
        // std::string pass = "horwEh-2zexmi-gyjvix";
        // mongocxx::client conn {
        // mongocxx::uri{"mongodb://"+username+":"+pass+"@ds042607.mlab.com:42607/project5"} };
        // auto collection = conn["project5"]["Twitter_collection"]; //[Database][collection]

        mongocxx::cursor cursor = collection.find(bsoncxx::builder::stream::document{} << "Key" << term << finalize);
            for(auto doc : cursor) {
              std::cout << bsoncxx::to_json(doc) << "\n";
              request.reply(web::http::status_codes::OK, bsoncxx::to_json(doc), "text/javascript");
            }
    }
}

int main(int argc, char *argv[]) {

	// mongocxx::instance inst{};
	// std::string username = "eashan22";
	// std::string pass = "horwEh-2zexmi-gyjvix";
	// mongocxx::client conn {
	// mongocxx::uri{"mongodb://"+username+":"+pass+"@ds042607.mlab.com:42607/project5"} };
	// auto collection = conn["project5"]["Twitter_collection"]; //[Database][collection]
	  
	utility::string_t address = U("http://172.17.0.2:");
	address.append(U("34568"));
	web::uri_builder uri(address);
	std::string addr = uri.to_uri().to_string();
	web::http::experimental::listener::http_listener listener(addr);
	listener.support(web::http::methods::GET, handle_get);
	  // listener.support(web::http::methods::GET, [&](web::http::http_request request) {
	  //   handle_GET(request, collection);
	  // });

	listener
	.open()
	.then([]() {std::cout << "Listening on http://localhost:34568/\n";})
	.wait();

	std::string line;
	std::getline(std::cin, line);

	listener.close().wait();

	return 0;
}
