//********************************
// MongoDB Prerequisites
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
#include <bsoncxx/builder/stream/helpers.hpp>

#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/value.h>


//********************************
//For twitter-lient interaction part
#include <string>
#include <iostream>
#include <getopt.h>
#include <cstdio>
#include <fstream>
#include "include/twitcurl.h"
#include "urlencode.cpp"

using namespace mongocxx;
using namespace bsoncxx;
using std::endl; 
using std::cout;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

void printUsage()
{
    printf( "\nUsage:\ntwitterClient -u username -p password\n" );
}

int main( int argc, char* argv[] )
{
//  /* Get username and password from command line args */
    std::string userName( "@eashan2212" );
    std::string passWord( "sairam2212" );

    twitCurl twitterObj;
    std::string tmpStr, tmpStr2;
    std::string result, l_tmpStr, Result;
    int x;
    std::string replyMsg;
    char tmpBuf[1024];

//  /* Set twitter username and password */
    twitterObj.setTwitterUsername( "@eashan2212" );
    twitterObj.setTwitterPassword( "sairam2212" );

//  /* OAuth flow begins */
//  /* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
    twitterObj.getOAuth().setConsumerKey( std::string( "RcS1lTPZjcVRXl2GSczfVReFS" ) );
    twitterObj.getOAuth().setConsumerSecret( std::string( "q8R3eUipcREUw2sin2vbmVveiWjdPSxpDiwGE2fk7eWcXHx5Tf" ) );

//  /* Step 1: Check if we alredy have OAuth access token from a previous run */
    std::string myOAuthAccessTokenKey("");
    std::string myOAuthAccessTokenSecret("");
    std::ifstream oAuthTokenKeyIn;
    std::ifstream oAuthTokenSecretIn;

    oAuthTokenKeyIn.open( "twitterClient_token_key.txt" );
    oAuthTokenSecretIn.open( "twitterClient_token_secret.txt" );

    memset( tmpBuf, 0, 1024 );
    oAuthTokenKeyIn >> tmpBuf;
    myOAuthAccessTokenKey = tmpBuf;

    memset( tmpBuf, 0, 1024 );
    oAuthTokenSecretIn >> tmpBuf;
    myOAuthAccessTokenSecret = tmpBuf;

    oAuthTokenKeyIn.close();
    oAuthTokenSecretIn.close();

    if( myOAuthAccessTokenKey.size() && myOAuthAccessTokenSecret.size() )
    {
//      /* If we already have these keys, then no need to go through auth again */
        printf( "\nUsing:\nKey: %s\nSecret: %s\n\n", myOAuthAccessTokenKey.c_str(), myOAuthAccessTokenSecret.c_str() );

        twitterObj.getOAuth().setOAuthTokenKey( myOAuthAccessTokenKey );
        twitterObj.getOAuth().setOAuthTokenSecret( myOAuthAccessTokenSecret );
    }
    else
    {
//      /* Step 2: Get request token key and secret */
        std::string authUrl;
        twitterObj.oAuthRequestToken( authUrl );

//      /* Step 3: Get PIN  */
        memset( tmpBuf, 0, 1024 );
        printf( "\nDo you want to visit twitter.com for PIN (0 for no; 1 for yes): " );
        fgets( tmpBuf, sizeof( tmpBuf ), stdin );
        tmpStr = tmpBuf;
        if( std::string::npos != tmpStr.find( "1" ) )
        {
//          /* Ask user to visit twitter.com auth page and get PIN */
            memset( tmpBuf, 0, 1024 );
            printf( "\nPlease visit this link in web browser and authorize this application:\n%s", authUrl.c_str() );
            printf( "\nEnter the PIN provided by twitter: " );
            fgets( tmpBuf, sizeof( tmpBuf ), stdin );
            tmpStr = tmpBuf;
            twitterObj.getOAuth().setOAuthPin( tmpStr );
        }
        else
        {
//          /* Else, pass auth url to twitCurl and get it via twitCurl PIN handling */
            twitterObj.oAuthHandlePIN( authUrl );
        }

//      /* Step 4: Exchange request token with access token */
        twitterObj.oAuthAccessToken();

//      /* Step 5: Now, save this access token key and secret for future use without PIN */
        twitterObj.getOAuth().getOAuthTokenKey( myOAuthAccessTokenKey );
        twitterObj.getOAuth().getOAuthTokenSecret( myOAuthAccessTokenSecret );

//      /* Step 6: Save these keys in a file or wherever */
        std::ofstream oAuthTokenKeyOut;
        std::ofstream oAuthTokenSecretOut;

        oAuthTokenKeyOut.open( "twitterClient_token_key.txt" );
        oAuthTokenSecretOut.open( "twitterClient_token_secret.txt" );

        oAuthTokenKeyOut.clear();
        oAuthTokenSecretOut.clear();

        oAuthTokenKeyOut << myOAuthAccessTokenKey.c_str();
        oAuthTokenSecretOut << myOAuthAccessTokenSecret.c_str();

        oAuthTokenKeyOut.close();
        oAuthTokenSecretOut.close();
    }

//     /* OAuth flow ends */
    /* Account credentials verification */
    if( twitterObj.accountVerifyCredGet() )
    {
        twitterObj.getLastWebResponse( replyMsg );
        printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet web response:\n%s\n", replyMsg.c_str() );
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet error:\n%s\n", replyMsg.c_str() );
    }


    
    /* Search a string */
    printf( "\nEnter string to search: " );
    memset( tmpBuf, 0, 1024 );
    fgets( tmpBuf, sizeof( tmpBuf ), stdin );
    tmpStr = tmpBuf;
    printf( "\nLimit search results to: " );
    memset( tmpBuf, 0, 1024 );
    fgets( tmpBuf, sizeof( tmpBuf ), stdin );
    tmpStr2 = tmpBuf;

    // This function needs implementation for search terms with alpha-numeric characters before and after the term.
    replyMsg = "";
    result = urlencode(tmpStr);
    printf("result: '%s'\n", result.c_str());

    l_tmpStr = strlen(result.c_str());
    Result = result.substr(0, result.size()-3);
    tmpStr2 = tmpStr2.substr(0, tmpStr2.size()-1);

    if( twitterObj.search(Result,tmpStr2))
    {
        twitterObj.getLastWebResponse( replyMsg );
        printf( "\ntwitterClient:: twitCurl::search web response:\n%s\n", replyMsg.c_str() );
        
        //Saving the file
        Json::Value root;   
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(replyMsg.c_str(), root);

        std::ofstream out;
        out.open("twitter_data.json");
        out << root;
        out.close();
        
        //Creating instance of MongoDB. 
        mongocxx::instance inst{};
        std::string username = "eashan22";
        std::string pass = "horwEh-2zexmi-gyjvix";
        mongocxx::client conn {
        mongocxx::uri{"mongodb://"+username+":"+pass+"@ds042607.mlab.com:42607/project5"} };
        //exit(0);
        auto collection = conn["project5"]["Twitter_collection"]; //[Database][collection]

        std::cout << "Creating a simple document \n";
        // bsoncxx::builder::basic::document doc{};
         
        //Create a funtion which can parse json for key and values.
        std::vector<bsoncxx::document::value> documents;
        for(int i = 0; i < 101; i++) {
            std::string date = root["statuses"][i]["created_at"].asString();
            documents.push_back(
            bsoncxx::builder::stream::document{} << "Key" << Result << "date" <<  date << finalize);
            //Date is not popping into the mlab. 
        }
        collection.insert_many(documents);
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::search error:\n%s\n", replyMsg.c_str() );
    }


    return 0;
}

