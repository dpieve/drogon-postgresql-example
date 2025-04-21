#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class product : public drogon::HttpController<product>
{
public:
  METHOD_LIST_BEGIN

  METHOD_ADD(product::getAll, "list", Get);
  METHOD_ADD(product::read, "/id={1:id}", Get); // path: /product/id={id}
  METHOD_ADD(product::create, "/create", Post);
  METHOD_ADD(product::update, "/update/{1:id}", Put);
  METHOD_ADD(product::remove, "/remove/{1:id}", Delete);

  METHOD_LIST_END

private:

  void getAll(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
  void read(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id);
  void create(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
  void update(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id);
  void remove(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id);

  HttpResponsePtr makeResponse(const drogon::HttpStatusCode code, const std::string& message);
};
