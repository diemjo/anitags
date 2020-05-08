#include "../inc/httplib.h"
#include "../inc/networkQuery.h"
#include <iostream>
#include <json/json.h>

using namespace std;

float getSimilarity(Json::Value root);
int getStatus(Json::Value root);
string getLink(Json::Value root);

char dbmaskarray[37] = {
        /*mangadex*/        '0',
        /*madikami*/        '0',
        /*pawoo*/           '0',
        /*da*/              '0',
        /*portalgraphics*/  '0',
        /*bcycosplay*/      '0',
        /*bcyillust*/       '0',
        /*idolcomplex*/     '0',
        /*e621*/            '0',
        /*animepictures*/   '0',
        /*sankaku*/         '1',
        /*konachan*/        '0',
        /*gelbooru*/        '1',
        /*shows*/           '0',
        /*movies*/          '0',
        /*hanime*/          '0',
        /*anime*/           '0',
        /*mediband*/        '0',
        /*2dmarket*/        '0',
        /*hmisc*/           '0',
        /*fakku*/           '0',
        /*shutterstock*/    '0',
        /*reserved*/        '0',
        /*animeop*/         '0',
        /*yandere*/         '0',
        /*nijie*/           '0',
        /*drawr*/           '0',
        /*danbooru*/        '0',
        /*seigaillust*/     '0',
        /*reserved*/        '0',
        /*pixivhistorical*/ '1',
        /*pixiv*/           '1',
        /*ddbsample*/       '0',
        /*ddbobjects*/      '0',
        /*hcg*/             '0',
        /*reserved*/        '0',
        /*hmags*/           '0'
};

httplib::SSLClient* client;

bool queryTags(string filename) {
    ifstream fin(filename, ios::binary);
    ostringstream sout;
    sout << fin.rdbuf();
    string content(sout.str());
    httplib::MultipartFormDataItems items = {
            { "file", content, filename, "image/png"}
    };
    int dbmask = std::strtol(dbmaskarray, NULL, 2);
    auto res = client->Post(("/search.php?output_type=2&numres=1&api_key=42d1922a529642b60bd3cf8ea1b934200e0f341a&dbmask="+to_string(dbmask)).c_str(), items);
    if (!res) {
        fprintf(stderr, "Error: https post request failed, response is null\n");
        return false;
    }
    if (res->status == 403) {
        fprintf(stderr, "Error: Incorrect or Invalid API key for saucenao.com\n");
        return false;
    }
    else if (res->status != 200) {
        fprintf(stderr, "Error: Post request returned status code %d\n", res->status);
        return false;
    }

    JSONCPP_STRING err;
    Json::Value root;
    Json::CharReaderBuilder builder;
    const char* body = res->body.c_str();
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(body, body + res->body.length(), &root,
                       &err)) {
        fprintf(stderr, "Error: could not parse json from html body\n");
        return false;
    }

    int saucenao_status = getStatus(root);
    if (saucenao_status!=0) {
        fprintf(stderr, "Error: saucenao.com returned status: %d\n", saucenao_status);
        return false;
    }
    string link = getLink(root);
    float similarity = getSimilarity(root);

    printf("%s: SOURCE=%s [%2.2f%%]\n", filename.c_str(), link.c_str(), similarity);
    //cout << filename << ": SOURCE=" << link << " [" << similarity << "%]" << endl;

    return true;
}

void initQueryTags() {
    client = new httplib::SSLClient("saucenao.com");
}

string getLink(Json::Value root) {
    const Json::Value data = root["results"][0]["data"];
    if (!data) return "";
    const Json::Value pixiv_id = data["pixiv_id"];
    if (pixiv_id)
        return "https://www.pixiv.net/member_illust.php?mode=medium&illust_id="+pixiv_id.asString();
    const Json::Value sankaku_id = data["sankaku_id"];
    if (sankaku_id)
        return "https://chan.sankakucomplex.com/post/show/"+sankaku_id.asString();
    const Json::Value gelbooru_id = data["gelbooru_id"];
    if (gelbooru_id)
        return "https://gelbooru.com/index.php?page=post&s=view&id="+gelbooru_id.asString();
    return "";
}


float getSimilarity(Json::Value root) {
    const Json::Value header = root["results"][0]["header"];
    if (!header) return -1;
    const string similarity = header.get("similarity", "-1").asString();
    return atof(similarity.c_str());
}

int getStatus(Json::Value root) {
    const Json::Value header = root["header"]["status"];
    if (!header) return -42069;
    return header.asInt();
}