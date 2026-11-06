/**
 * @file 49_libcurl_demo.c
 * @brief Performing a basic HTTP GET request using libcurl.
 */
#include <curl/curl.h>
#include <stdio.h>

int main(void) {
    printf("--- libcurl Demo ---\n");

    // Global initialization (should be done once per program lifetime)
    curl_global_init(CURL_GLOBAL_DEFAULT);

    CURL *curl = curl_easy_init();
    if (curl) {
        CURLcode res;
        const char *target_url = "http://example.com";

        printf("Fetching data from: %s\n\n", target_url);

        // Configure the URL and behavior
        curl_easy_setopt(curl, CURLOPT_URL, target_url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            printf("\ncurl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("\n\nRequest completed successfully.\n");
        }

        // Cleanup the easy handle
        curl_easy_cleanup(curl);
    }

    // Global cleanup
    curl_global_cleanup();
    return 0;
}
