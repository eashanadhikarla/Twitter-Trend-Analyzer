#include <cstdio>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <ctime>
#include <jsoncpp/json/json.h>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "include/twitcurl.h"

using namespace std;
using namespace bsoncxx::builder::stream;

void usage(const char *progname)
{
    cout << "Search a term in Twitter and put the results into MongoDB\n";
    cout << "  Usage: " << progname << " [options]\n";
    cout << "    -u <string>   : twitter username (wanzhao0909@gmail.com)\n";
    cout << "    -p <string>   : twitter password (******)\n";
    cout << "    -s <string>   : string to be searched (Iphone7)\n";
    cout << "    -n <string>   : number of results (100)\n";
    cout << "    -c            : Clear the mongoDB\n";
}

void save_to_mongo(Json::Value json_res){
    string username = "eashan22";
    string pass = "horwEh-2zexmi-gyjvix";
    string info="Inserted";

    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{"mongodb://"+username+":"+pass+"@ds042607.mlab.com:42607/project5"}};

    auto collection = conn["project5"]["Twitter_collection"];

/**********************************************************************/

    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result = 
    collection.find_one(document{} << "term" << json_res["search_string"].asString() << finalize);
    if(maybe_result) {
      bsoncxx::stdx::optional<mongocxx::result::delete_result> result =
      collection.delete_many(
        document{} << "term" << json_res["search_string"].asString() << finalize);
      info = "Already In -> Updated";
  }
/**********************************************************************/

  vector<bsoncxx::document::value> documents;
  for(int i = 0; i < json_res["histogram"].size(); i++) {
    documents.push_back(document{} << "term" << json_res["search_string"].asString()
     << "date" << json_res["histogram"][i]["date"].asString()
     << "num"  << json_res["histogram"][i]["num"].asString()
     << finalize);
}
collection.insert_many(documents);


cout << info << endl;

/**********************************************************************/
auto cursor = collection.find({});

for (auto&& doc : cursor) {
    cout << bsoncxx::to_json(doc) << endl;
}

}

void clear_mongo(){
    string username = "eashan22";
    string pass = "horwEh-2zexmi-gyjvix";
    string info="Inserted";

    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{"mongodb://"+username+":"+pass+"@ds042607.mlab.com:42607/project5"}};

    auto collection = conn["project5"]["Twitter_collection"];

    collection.delete_many(document{} << finalize);

    cout << "Database Cleared" << endl;

}


string word2num_month(string mon_word){
    string mon_int;
    if (mon_word == "Jan")
        mon_int = "1";
    else if (mon_word == "Feb")
        mon_int = "2";
    else if (mon_word == "Mar")
        mon_int = "3";
    else if (mon_word == "Apr")
        mon_int = "4";
    else if (mon_word == "May")
        mon_int = "5";
    else if (mon_word == "Jun")
        mon_int = "6";
    else if (mon_word == "Jul")
        mon_int = "7";
    else if (mon_word == "Aug")
        mon_int = "8";
    else if (mon_word == "Sep")
        mon_int = "9";
    else if (mon_word == "Oct")
        mon_int = "10";
    else if (mon_word == "Nov")
        mon_int = "11";
    else if (mon_word == "Dec")
        mon_int = "12";
    return mon_int;
}

int main( int argc, char* argv[] )
{
    /* Get username and password from command line args */
    // I can still get the results even though the userName doesn't match the passWord
    string userName = "aeashan@gmail.com";
    string passWord = "sairam2212";
    string search_string = "Iphone7";
    string num_res = "100";
    int num_res_int = stoi(num_res);
    bool show_help = false;
    ofstream fout;
    // Parse the command line options:
    int o;
    while((o = getopt(argc, argv, "u:p:s:n:hc")) != -1){
        switch (o){
            case 'u':
            userName = optarg;
            break;
            case 'p':
            passWord = optarg;
            break;
            case 's':
            search_string = optarg;
            break;
            case 'n':
            num_res = optarg;
            num_res_int = stoi(num_res);
            break;
            case 'h':
            show_help = true;
            break;
            case 'c':
            clear_mongo();
            return 1;
            break;
            default:
            show_help = true;
            break;
        }
    }
    // Print help and exit
    if (show_help) {
        usage(basename(argv[0]));
        exit(0);
    }

    twitCurl twitterObj;
    string tmpStr, tmpStr2;
    string replyMsg;
    char tmpBuf[1024];
    /* Set twitter username and password */
    twitterObj.setTwitterUsername( userName );
    twitterObj.setTwitterPassword( passWord );

    /* OAuth flow begins */
    /* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
    twitterObj.getOAuth().setConsumerKey( std::string( "RcS1lTPZjcVRXl2GSczfVReFS" ) );
    twitterObj.getOAuth().setConsumerSecret( std::string( "q8R3eUipcREUw2sin2vbmVveiWjdPSxpDiwGE2fk7eWcXHx5Tf" ) );

    /* Step 1: Check if we alredy have OAuth access token from a previous run */
    std::string myOAuthAccessTokenKey("2971030992-ET8inyO9Y9Z4zXHCvvcN3guRwUmq0VYA1Uph40A");
    std::string myOAuthAccessTokenSecret("cI5NcMBT79fhQJFHHLKgBTUy1HDXZu7YvT68I2gMC9Y9P");

    /* If we already have these keys, then no need to go through auth again */
    cout << "*************************************************************" << endl;
    cout << "Search string: " << search_string << endl;
    cout << "Num of Result: " << num_res << endl;
    cout << "*************************************************************" << endl;

    twitterObj.getOAuth().setOAuthTokenKey( myOAuthAccessTokenKey );
    twitterObj.getOAuth().setOAuthTokenSecret( myOAuthAccessTokenSecret );
    /* OAuth flow ends */

    /* Search a string */
    replyMsg = "";
    if( twitterObj.search( search_string, num_res ) )
    {
        twitterObj.getLastWebResponse( replyMsg );
        // printf( "\ntwitterClient:: twitCurl::search web response:\n%s\n", replyMsg.c_str() );
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        // printf( "\ntwitterClient:: twitCurl::search error:\n%s\n", replyMsg.c_str() );
    }

    /* save as json file */
    string out, date_his;
    int num_found, num_date_his, count_date_his = 0;
    Json::Reader reader;
    Json::Value root,res_root;
    Json::FastWriter writer;
    
    reader.parse(replyMsg, root);
    num_found = root["statuses"].size();
    if (num_found == 0){
        cout << "No Results Found" << endl;
        // return 0;
    }
    res_root["num_found"] = num_found;
    res_root["search_string"] = search_string;
    cout << "Num Found: " << num_found << endl;
    cout << "Print Top " << (10<num_found?10:num_found) << " Results" << endl;
    for (int i = 0; i < (10<num_found?10:num_found); ++i)
    {
        cout << root["statuses"][i]["text"] << endl;
    }
    cout << "*************************************************************" << endl;

    for (int i = 0; i < num_found; ++i)
    {
        string date, hour, min, sec, day, month, year, created;
        created = root["statuses"][i]["created_at"].asString();
        sec = created.substr(17, 2);
        min = created.substr(14, 2);
        hour = created.substr(11, 2);
        day = created.substr(8, 2);
        month = created.substr(4, 3);
        month = word2num_month(month);
        year = created.substr(26);
        date = year + "/" + month + "/" + day;
        
        res_root["res"][i]["sec"] = Json::Value(sec);
        res_root["res"][i]["min"] = Json::Value(min);
        res_root["res"][i]["hour"] = Json::Value(hour);
        res_root["res"][i]["day"] = Json::Value(day);
        res_root["res"][i]["month"] = Json::Value(month);
        res_root["res"][i]["year"] = Json::Value(year);
        res_root["res"][i]["date"] = Json::Value(date);

        if (i == 0)
        {
            date_his = date;
            num_date_his = 0;
        }


        if (date != date_his)  // detect a new date
        {
            res_root["histogram"][count_date_his]["date"] = date_his;
            res_root["histogram"][count_date_his++]["num"] = num_date_his;
            date_his = date;
            num_date_his = 1;
        }
        else{
            num_date_his++;
        }
    }

    res_root["histogram"][count_date_his]["date"] = date_his;
    res_root["histogram"][count_date_his++]["num"] = num_date_his;

    out = root.toStyledString();
    fout.open("raw.json");
    fout << out;
    fout.close();
    fout.open("res.json");
    fout << res_root;
    fout.close();

    save_to_mongo(res_root);

    return 0;
}
