#include <drogon/drogon.h>

int main() {
    drogon::app()
        .loadConfigFile("config.json")
        .registerPostHandlingAdvice(
                [](const drogon::HttpRequestPtr& req, const drogon::HttpResponsePtr& resp) {
                    resp->addHeader("Access-Control-Allow-Origin", "*");
                })
        .run();
    return 0;
}
