/*
LIBRARY/DOCS HELP:
https://curl.haxx.se

JSON-PARSER:
*/

#include "randomissue.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

//To compile: gcc -g -o RandomIssue randomissue.c -lcurl -ljson-c

int main()
{
    //!!!!!!!!!!!!!!! REMEBER TO FREEEEEE OUR STUFF
    char *repoPath;

    printf("Enter the repo & path with (/ included) 'author/name': ");
    scanf("%s", repoPath);
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();

    if (!curl)
        return 0;

    struct json_object *json_list = NULL;

    printf("Getting random issue...\n");

    int parse = 1, index = 0;
    while (parse)
    {
        char *url;
        if (0 > asprintf(&url, "https://api.github.com/repos/%s/issues?state=open&page=%d&per_page=100", repoPath, index + 1))
            return 0;

        struct json_object *current_json = getJsonFromURL(curl, res, index, url, "rhin123");

        if (json_object_array_length(current_json) > 0)
            json_list = concatJson(json_list, current_json);
        else
            parse = 0;

        index++;
    }

    if (!json_list)
    {
        //TODO: BETTER ERROR CHECKING.
        /*
        Errors to check:
        - No errors on repo (Empty json)
        - Repo not found
        - API Limit reached
         */
        fprintf(stderr, "Error: JSON is NULL. Our API Limit has probally been reached.\n");
        return 0;
    }

    printGitIssue(getRandomIssue(json_list));

    curl_easy_cleanup(curl);
    free(json_list);
    return 1;
}

struct json_object *getJsonFromURL(CURL *curl, CURLcode res, int index, char *url, char *username)
{
    struct json_object *parsed_json;

    String s;
    init_string(&s);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, username); //TODO: Probally should ask user to specifiy username.

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    res = curl_easy_perform(curl);

    //Init our json
    parsed_json = json_tokener_parse(s.ptr);

    free(s.ptr);

    return parsed_json;
}

GitIssue getRandomIssue(struct json_object *parsed_json)
{
    srand(time(0));
    int rnd_number = rand() % json_object_array_length(parsed_json) + 0;
    struct json_object *current_issue;
    current_issue = json_object_array_get_idx(parsed_json, rnd_number);

    GitIssue issue;
    json_object_object_get_ex(current_issue, "url", &issue.url);
    json_object_object_get_ex(current_issue, "html_url", &issue.html_url);
    json_object_object_get_ex(current_issue, "title", &issue.title);
    json_object_object_get_ex(current_issue, "body", &issue.body);
    json_object_object_get_ex(current_issue, "number", &issue.number);

    return issue;
}

void printGitIssue(GitIssue issue)
{
    printf("Issue #%d: %s\n", json_object_get_int(issue.number), json_object_get_string(issue.html_url));
    printf("%s\n", json_object_get_string(issue.title));
    //printf("%s\n", json_object_get_string(issue.body));
}

struct json_object *concatJson(struct json_object *array1, struct json_object *array2)
{
    struct json_object *array3 = json_object_new_array();
    if (!array3)
        return NULL;

    if (!array1)
        return array2;

    if (!array2)
        return array1;

    int i,
        size1 = json_object_array_length(array1),
        size2 = json_object_array_length(array2);
    for (i = 0; i < size1; ++i)
    {
        json_object_array_add(array3, json_object_array_get_idx(array1, i));
    }
    for (i = 0; i < size2; ++i)
    {
        json_object_array_add(array3, json_object_array_get_idx(array2, i));
    }
    return array3;
}

void init_string(String *s)
{
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    if (s->ptr == NULL)
    {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, String *s)
{
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL)
    {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}
