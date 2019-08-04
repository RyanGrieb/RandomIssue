//Allows us to use asprintf
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <curl/curl.h>
#include <curl/easy.h>

#include <string.h>
#include <json-c/json.h>

typedef struct String //TODO: USE json_objectr in our string struct?
{
    char *ptr;
    size_t len;
} String;

typedef struct GitIssue
{
    json_object *url;
    json_object *html_url;
    json_object *title;
    json_object *body;

    json_object *number;
} GitIssue;

struct json_object *getJsonFromURL(CURL *curl, CURLcode res, int index, char *url, char *username);
GitIssue getRandomIssue(struct json_object *parsed_json);
void printGitIssue(GitIssue issue);
void init_string(String *s);
struct json_object *concatJson(struct json_object *array1, struct json_object *array2); //https://stackoverflow.com/questions/36998026/append-json-object-arrays-in-c-using-jsonc-library
size_t writefunc(void *ptr, size_t size, size_t nmemb, String *s);