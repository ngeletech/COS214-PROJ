#ifndef ADAPTER
#define ADAPTER

#include <string>
#include <map>

#include "WorkData.h"

using namespace std;

/**
 * @class ServiceRequest
 * @brief Data class representing the request sent to an external service
 */
class ServiceRequest {
    private:
        /**
         * @brief The data sent to the external service
         */
        WorkData payload;
    
    public:
        /**
         * @brief Cosntructs a request with the given payload
         * @param payload The data to send
         */
        ServiceRequest(WorkData payload);

        /**
         * @brief Returns the payload
         * @return A constant reference to the internal WorkData
         */
        const WorkData& getPayload() const;
};

/**
 * @class ServiceResponse
 * @brief Data class representing the result of an external service request
 */
class ServiceResponse {
    private:
        /**
         * @brief The external system's tracking ID
         */
        string reference;

        /**
         * @brief A readable status
         */
        string status;

        /**
         * @brief True if the request was accepted
         */
        bool ok;

    public:
        /**
         * @brief Constructs a response
         * @param reference The external tracking ID
         * @param status The human-readable status
         * @param ok True if accepted, false otherwise
         */
        ServiceResponse(string reference, string status, bool ok);

        /**
         * @brief Returns the tracking ID
         * @return The reference string
         */
        string getReference() const;

        /**
         * @brief Returns the readable status
         * @return The status string
         */
        string getStatus() const;

        /**
         * @brief Returns whether the request was accepted or not
         * @return Ture if accepted, false otherwise
         */
        bool isOk() const;
};

/**
 * @class ExternalServiceConnector
 * @brief The target interface. Defines how TVIDY talks to external systems
 */
class ExternalServiceConnector {
    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~ExternalServiceConnector() {};

        /**
         * @brief Sends a request to the external system
         * @param req The request to send
         * @return A response indicating success and the tracking ID
         */
        virtual ServiceResponse sendRequest(const ServiceRequest& req) = 0;

        /**
         * @brief Queries the status of a previously sent request
         * @param reference The tracking ID from a prior sendRequest()
         * @return The readable status string
         */
        virtual string queryStatus(string reference) = 0;

};

/**
 * @class RestApiClient
 * @brief Adaptee: a REST-style web client
 */

class RestApiClient {
    private:
        /**
         * @brief Used to generate unique reference IDs
         */
        static int counter;

        /**
         * @brief Maps reference IDs to their statuses
         */
        std::map<string, string> statuses;

        /**
         * @brief Determines if a specific request ID should fail
         * @param id The numeric ID
         * @return True if it is a failing ID
         */
        static bool isFailingId(int id) {
            return id % 10 == 0;    
        }

    public:
        /**
         * @brief Sends a POST request
         * @param url The endpoitn URL
         * @param body The request body
         * @return A generated reference ID
         */
        string post(string url, string body);

        /**
         * @brief Sends a GET request to check a status
         * @param url The endpoint URL with the reference ID
         * @return The status string
         */
        string get(string url);
};

/**
 * @class RestApiAdapter
 * @brief Adapts RestApiClient to the ExternalServiceConnector interface
 */
class RestApiAdapter: public ExternalServiceConnector {
    private:
        /**
         * @brief The adaptee (not owned)
         */
        RestApiClient* client;

    public:
        /**
         * @brief Constructs the adapter.
         * @param client The RestApiClient to wrap. Must outlive the adapter.
         */
        RestApiAdapter(RestApiClient* client = nullptr);

        /**
         * @brief Destructor (does not delete the borrowed client).
         */
        ~RestApiAdapter();
        ServiceResponse sendRequest(const ServiceRequest& req);
        string queryStatus(string reference);
};

/**
 * @class LegacySystemApi
 * @brief Adaptee: an old system that uses fixed-width string codes
 */
class LegacySystemApi {
    private:
        /**
         * @brief A submission counter
         */
        static int counter;

        /**
         * @brief Maps codes to lookup counts
         */
        std::map<std::string, int> lookups;

    public:
        /**
         * @brief Submits a fixed-width record to the legacy system
         * @param record The encoded data
         * @return A status code
         */
        string submitRecord(string record);

        /**
         * @brief Looks up the status of a submitted code
         * @param code The code returned by submitRecord();
         * @return A two-digit legacy status code
         */
        string lookup(string code);
};

/** @class RestApiAdapter
 * @brief Adapts RestApiClient to the ExternalServiceConnector interface.
 */
class LegacySystemAdapter: public ExternalServiceConnector {
    private:
        /**
         * @brief The adaptee (not owned)
         */
        LegacySystemApi* api;

    public:
        /**
         * @brief Constructs the adapter.
         * @param client The RestApiClient to wrap. Must outlive the adapter.
         */
        LegacySystemAdapter(LegacySystemApi* api = nullptr);

        /**
         * @brief Destructor (does not delete the borrowed client).
         */
        ~LegacySystemAdapter();
        ServiceResponse sendRequest(const ServiceRequest& req);
        string queryStatus(string reference);
};

/**
 * @class MessageQueueClient
 * @brief Adaptee: a message queue broker
 */
class MessageQueueClient {
    private:
        /**
         * @brief Maps correlation IDs to statuses
         */
        std::map<std::string, std::string> messages;

    public:
        /**
         * @brief Publishes a message to a subject
         * @param subject The topic name
         * @param message The message that is expected to contain "id|body"
         */
        void publish(string subject, string message);

        /**
         * @brief Polls the queue for teh status of a correlation ID
         * @param correlationId The ID to look up
         * @return The status string
         */
        string poll(string correlationId);
};

/**
 * @class MessagingAdapter
 * @brief Adapts MessageQueueClient to the ExternalServiceConnector interface.
 */
class MessagingAdapter: public ExternalServiceConnector {
    private:
        /**
         * @brief The adaptee (held by value)
         */
        MessageQueueClient queue;

        /**
         * @brief Used to generate unique message IDs
         */
        static int counter;

    public:
        ServiceResponse sendRequest(const ServiceRequest& req);
        string queryStatus(string reference);
};

#endif //ADAPTER