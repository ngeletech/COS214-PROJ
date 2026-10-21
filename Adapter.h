#ifndef ADAPTER
#define ADAPTER

#include <string>

using namespace std;

class ServiceRequest {
    private:
        WorkData payload;
    
    public:
        ServiceRequest(Workdata payload);
        Workdata getPayload();
};

class ServiceResponse {
    private:
        string reference;
        string status;

    public:
        ServiceResponse(string reference, string status);
        string getReference();
        string getStatus();
};

class ExternalServiceConnector {
    public:
        virtual ~ExternalServiceConnector();
        virtual ServiceResponse sendRequest(const ServiceRequest& req) = 0;
        virtual string queryStatus(string reference) = 0;

};

class RestApiClient {
    public:
        string post(string url, string body);
        string get(string url);
};

class RestApiAdapter: public ExternalServiceConnector {
    private:
        RestApiClient* client;

    public:
        ServiceResponse sendRequest(const ServiceRequest& req);
        string queryStatus(string reference);
};

class LegacySystemApi {
    public:
        string submitRecord(string record);
        string lookup(string code);
};

class LegacySystemAdapter: public ExternalServiceConnector {
    private:
        LegacySystemApi* api;

    public:
        ServiceResponse sendRequest(const ServiceRequest& req);
        string queryStatus(string reference);
};

class MessageQueueClient {
    public:
        void publish(string subject, string message);
        string poll(string correlationId);
};

class MessagingAdapter: public ExternalServiceConnector {
    private:
        MessageQueueClient queue;

    public:
        ServiceResponse sendRequest(const ServiceRequest& req);
        string queryStatus(string reference);
};

#endif //ADAPTER