#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <sys/time.h>

// Function to get current time in milliseconds
long long current_time_in_milliseconds() {
    struct timeval te;
    gettimeofday(&te, NULL);
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
    return milliseconds;
}

// Function to check the availability and response time of a web service
void check_web_service(const char* url) {
    CURL *curl;
    CURLcode res;
    long response_code;
    double total_time;
    long long start_time, end_time;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1); // We don't need the body, just the response

        start_time = current_time_in_milliseconds();
        res = curl_easy_perform(curl);
        end_time = current_time_in_milliseconds();

        if(res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);

            printf("Service URL: %s\n", url);
            printf("Response Code: %ld\n", response_code);
            printf("Total Time: %.2f ms\n", total_time * 1000);
            printf("Measured Time: %lld ms\n\n", end_time - start_time);
        } else {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

int main() {
    // Check availability and response time for example services
    check_web_service("http://www.example.com");
    check_web_service("http://localhost:8080"); // Change to your local web service
    return 0;
}
