#include "product.h"
#include <jsoncpp/json/json.h>
#include <drogon/orm/Exception.h>

void product::getAll(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    try
    {
        auto clientPtr = drogon::app().getDbClient();
        auto result = clientPtr->execSqlSync("SELECT * FROM products");
        if (result.empty())
        {
            callback(makeResponse(k404NotFound, "No products found."));
            return;
        }

        Json::Value ret;
        for (const auto& row : result)
        {
            Json::Value item;
            item["id"] = row["id"].as<int>();
            item["name"] = row["name"].as<std::string>();
            item["price"] = row["price"].as<double>();
            ret.append(item);
        }

        callback(HttpResponse::newHttpJsonResponse(ret));
    }
    catch (const std::exception& e)
    {
        callback(makeResponse(k500InternalServerError, "Internal server error."));
    }
}

void product::read(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id)
{
    try
    {
        auto clientPtr = drogon::app().getDbClient();
        auto result = clientPtr->execSqlSync("SELECT * FROM products where id=$1", id);

        Json::Value ret;
        if (result.empty())
        {
            callback(makeResponse(k404NotFound, "Product not found."));
            return;
        }

        auto row = result[0];
        ret["id"] = row["id"].as<int>();
        ret["name"] = row["name"].as<std::string>();
        ret["price"] = row["price"].as<double>();

        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }
    catch (const std::exception& e)
    {
        callback(makeResponse(k500InternalServerError, "Internal server error."));
    }
}

void product::create(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    auto jsonObj = req->jsonObject();
    if (jsonObj == nullptr ||
        !jsonObj->isMember("name") ||
        !jsonObj->isMember("price"))
    {
        callback(makeResponse(k400BadRequest, "Invalid request. Fields not found."));
        return;
    }

    try
    {
        std::string name = jsonObj->get("name", Json::Value()).asString();
        double price = jsonObj->get("price", Json::Value()).asDouble();

        auto clientPtr = drogon::app().getDbClient();
        auto result = clientPtr->execSqlSync("insert into products (name, price) values ($1, $2)", name, price);
        if (result.affectedRows() == 0)
        {
            callback(makeResponse(k500InternalServerError, "Failed to create product."));
            return;
        }
        callback(makeResponse(k200OK, "Product created successfully."));
    }
    catch (const std::exception& e)
    {
        callback(makeResponse(k400BadRequest, "Invalid request. Failed to save."));
    }

}

void product::update(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id)
{
    auto jsonObj = req->jsonObject();
    if (jsonObj == nullptr ||
        !jsonObj->isMember("name") ||
        !jsonObj->isMember("price"))
    {
        callback(makeResponse(k400BadRequest, "Invalid request. Fields not found."));
        return;
    }

    try
    {
        std::string name = jsonObj->get("name", Json::Value()).asString();
        double price = jsonObj->get("price", Json::Value()).asDouble();

        auto clientPtr = drogon::app().getDbClient();
        auto result = clientPtr->execSqlSync("UPDATE products SET name=$1, price=$2 WHERE id=$3",
            name, price, id);
        if (result.affectedRows() == 0)
        {
            callback(makeResponse(k404NotFound, "Product not found."));
            return;
        }

        callback(makeResponse(k200OK, "Product updated successfully."));
    }
    catch (const std::exception& e)
    {
        callback(makeResponse(k400BadRequest, "Invalid request. Failed to update."));
    }
}

void product::remove(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id)
{
    try
    {
        Json::Value ret;

        auto clientPtr = drogon::app().getDbClient();
        auto result = clientPtr->execSqlSync("DELETE FROM products WHERE id=$1", id);

        if (result.affectedRows() == 0)
        {
            callback(makeResponse(k404NotFound, "Product not found."));
            return;
        }
        callback(makeResponse(k200OK, "Product deleted successfully."));
    }
    catch (const std::exception& e)
    {
        callback(makeResponse(k500InternalServerError, "Internal server error."));
    }
}

HttpResponsePtr product::makeResponse(const drogon::HttpStatusCode code, const std::string& message)
{
    Json::Value json;
    json["ok"] = code == k200OK || k201Created || k202Accepted;
    json["message"] = message;
    auto resp = HttpResponse::newHttpJsonResponse(json);
    resp->setStatusCode(code);
    return resp;
}