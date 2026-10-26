#ifndef ADAPTER
#define ADAPTER

#include <string>
#include <map>

class WorkData;

using namespace std;

class ServiceRequest {
    private:
        WorkData payload;
    
    public:
        ServiceRequest(WorkData payload);
        const WorkData& getPayload() const;
};

class ServiceResponse {
    private:
        string reference;
        string status;
        bool ok;

    public:
        ServiceResponse(string reference, string status, bool ok);
        string getReference();
        string getStatus();
        bool isOk();
};

class ExternalServiceConnector {
    public:
        virtual ~ExternalServiceConnector();
        virtual ServiceResponse sendRequest(const ServiceRequest& req) = 0;
        virtual string queryStatus(string reference) = 0;

};

class RestApiClient {
    private:
        static int counter;
        std::map<string, string> statuses;

        static bool isFailingId(int id) {
            return id % 10 == 0;    
        }
        
    public:
        string post(string url, string body);
        string get(string url);
};

class RestApiAdapter: public ExternalServiceConnector {
    private:
        RestApiClient* client;

    public:
        RestApiAdapter(RestApiClient* client);
        ~RestApiAdapter();
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