#include "RequestHandler.h"

#include <exception>

#include "Config.h"
#include "DatabaseFactory.h"
#include "PostgreDatabase.h"
#include "helper.h"
#include "loggerlib.h"
#include "messages/Char64str.h"
#include "messages/IdentityMessage.h"
#include "messages/MessageHeader.h"

namespace {

void log_identity(messages::IdentityMessage &identity) {
    qLogger::get().info_fast("msg: {}", identity.msg().getCharValAsString());
    qLogger::get().info_fast("type: {}", identity.type().getCharValAsString());
    qLogger::get().info_fast("id: {}", identity.id().getCharValAsString());
    qLogger::get().info_fast("name: {}", identity.name().getCharValAsString());
    qLogger::get().info_fast("dateOfIssue: {}",
                             identity.dateOfIssue().getCharValAsString());
    qLogger::get().info_fast("dateOfExpiry: {}",
                             identity.dateOfExpiry().getCharValAsString());
    qLogger::get().info_fast("address: {}",
                             identity.address().getCharValAsString());
    qLogger::get().info_fast("verified: {}",
                             identity.verified().getCharValAsString());
}

}  // namespace

RequestHandler::RequestHandler() noexcept {
    auto &cfg = Config::get();

    try {
        auto pgConfig = DatabaseConfig(cfg.DB_HOST, cfg.DB_PORT, cfg.DB_NAME,
                                       cfg.DB_USER, cfg.DB_PASSWORD);

        auto pgDb = DatabaseFactory::create("postgresql", pgConfig);
        _pgDbManager = std::make_unique<DatabaseManager>(std::move(pgDb));

        qLogger::get().info_fast("Connected to PostGreSQL");
    } catch (const std::exception &e) {
        qLogger::get().error_fast("Error: {}", e.what());
    }
}

RequestHandler::~RequestHandler() noexcept {}

std::vector<char> RequestHandler::respond(
    const aeron_wrapper::FragmentData &fragmentData) noexcept {
    std::vector<char> buffer;

    messages::MessageHeader msgHeader;
    char *base = reinterpret_cast<char *>(
        const_cast<uint8_t *>(fragmentData.atomicBuffer.buffer()));
    char *start = base + fragmentData.offset;

    msgHeader.wrap(start, 0, 0, fragmentData.length);
    size_t offset = msgHeader.encodedLength();

    if (msgHeader.templateId() == messages::IdentityMessage::sbeTemplateId()) {
        messages::IdentityMessage identity;
        identity.wrapForDecode(start, offset, msgHeader.blockLength(),
                               msgHeader.version(), fragmentData.length);
        log_identity(identity);

        std::string msgType = identity.msg().getCharValAsString();
        bool isVerified =
            string_to_bool(identity.verified().getCharValAsString());

        // Check if this is an "Identity Verification Request" with
        // verified=false
        if (msgType == "Identity Verification Request" && !isVerified) {
            std::string name = identity.name().getCharValAsString();
            std::string id = identity.id().getCharValAsString();

            qLogger::get().info_fast(
                "Processing Identity Verification Request for: {} {}", name,
                id);

            // Invoke verification method
            bool userExist = exist_user(id, name);

            if (userExist) {
                qLogger::get().info_fast("Verification successful for {} {}",
                                         name, id);
                // Send back verified message with verified=true
                buffer = get_buffer(identity, true);
            } else {
                qLogger::get().info_fast("Verification failed for {} {}", name,
                                         id);
                // Send back message with verified=false
                buffer = get_buffer(identity, false);
            }
        }
        // Check if this is an "Add User in System" request with verified=false
        else if (msgType == "Add User in System" && !isVerified) {
            std::string name = identity.name().getCharValAsString();
            std::string id = identity.id().getCharValAsString();

            qLogger::get().info_fast(
                "Processing Add User in System request for: {} {}", name, id);

            // Add user to database
            bool identityAdded = add_identity(identity);

            if (identityAdded) {
                qLogger::get().info_fast("User addition successful for {} {}",
                                         name, id);
                // Send back response with verified=true (user added
                // successfully)
                buffer = get_buffer(identity, true);
            } else {
                qLogger::get().info_fast("User addition failed for {} {}", name,
                                         id);
                // Send back response with verified=false (user addition failed)
                buffer = get_buffer(identity, false);
            }
        } else if (isVerified) {
            qLogger::get().info_fast("Identity already verified: {}",
                                     identity.name().getCharValAsString());
        } else {
            qLogger::get().info_fast("Message type '{}' - no action needed",
                                     msgType);
        }
    } else {
        qLogger::get().error_fast("[Decoder] Unexpected template ID: {}",
                                  msgHeader.templateId());
    }
    return buffer;
}

// Check if user exists in database
bool RequestHandler::exist_user(const std::string &identityNumber,
                                const std::string &name) noexcept {
    try {
        std::string selectQuery =
            "SELECT identity_number, name FROM users WHERE identity_number = "
            "'" +
            identityNumber + "' AND name = '" + name + "'";

        auto res = (*_pgDbManager)->exec(selectQuery);
        if (!res) {
            qLogger::get().error_fast("DB exec returned null");
            return false;
        }

        auto pgResult = dynamic_cast<PostgreResult *>(res.get());
        if (!pgResult) {
            qLogger::get().error_fast("Unexpected DB result type");
            return false;
        }

        bool exists = !pgResult->empty();

        qLogger::get().info_fast(
            exists ? "Verified: {} {} found in database"
                   : "NOT verified: {} {} not found in database",
            identityNumber, name);

        return exists;
    } catch (const std::exception &e) {
        qLogger::get().error_fast(
            "Database query error during user existence check: {}", e.what());
        return false;
    }
}

// Add user to database
bool RequestHandler::add_identity(
    messages::IdentityMessage &identity) noexcept {
    try {
        std::string type = identity.type().getCharValAsString();
        std::string identityNumber = identity.id().getCharValAsString();
        std::string name = identity.name().getCharValAsString();
        std::string dateOfIssue = identity.dateOfIssue().getCharValAsString();
        std::string dateOfExpiry = identity.dateOfExpiry().getCharValAsString();
        std::string address = identity.address().getCharValAsString();

        qLogger::get().info_fast(
            "Adding user to system: name={}, id={}, type={}", name,
            identityNumber, type);

        // Check if user already exists using the reusable method
        if (exist_user(identityNumber, name)) {
            qLogger::get().info_fast(
                "User already exists in system: {} {} ({})", name,
                identityNumber, type);
            return false;  // User already exists, don't add duplicate
        }

        qLogger::get().info_fast(
            "User not found in system, proceeding with addition: {} {}", name,
            identityNumber);

        // Insert user into database
        std::string insertQuery =
            "INSERT INTO users (type, identity_number, name, date_of_issue, "
            "date_of_expiry, address) "
            "VALUES ('" +
            type + "', '" + identityNumber + "', '" + name + "', '" +
            dateOfIssue + "', '" + dateOfExpiry + "', '" + address + "')";

        (*_pgDbManager)->exec(insertQuery);

        qLogger::get().info_fast(
            "User successfully added to system: {} {} ({})", name,
            identityNumber, type);

        return true;
    } catch (const std::exception &e) {
        qLogger::get().error_fast("Database error while adding user: {}",
                                  e.what());
        return false;
    }
}

// Send response message
std::vector<char> RequestHandler::get_buffer(
    messages::IdentityMessage &originalIdentity,
    bool verificationResult) noexcept {
    std::vector<char> buffer;
    try {
        using namespace messages;
        const size_t bufferCapacity =
            MessageHeader::encodedLength() + IdentityMessage::sbeBlockLength();
        buffer.resize(bufferCapacity);
        size_t offset = 0;

        // Encode header
        MessageHeader msgHeader;
        msgHeader.wrap(buffer.data(), offset, 0, bufferCapacity);
        msgHeader.blockLength(IdentityMessage::sbeBlockLength());
        msgHeader.templateId(IdentityMessage::sbeTemplateId());
        msgHeader.schemaId(IdentityMessage::sbeSchemaId());
        msgHeader.version(IdentityMessage::sbeSchemaVersion());
        offset += msgHeader.encodedLength();

        // Encode response message
        IdentityMessage identity;
        identity.wrapForEncode(buffer.data(), offset, bufferCapacity);

        // Copy original data but update verification status and message
        identity.msg().putCharVal("Identity Verification Response");
        identity.type().putCharVal(
            originalIdentity.type().getCharValAsString());
        identity.id().putCharVal(originalIdentity.id().getCharValAsString());
        identity.name().putCharVal(
            originalIdentity.name().getCharValAsString());
        identity.dateOfIssue().putCharVal(
            originalIdentity.dateOfIssue().getCharValAsString());
        identity.dateOfExpiry().putCharVal(
            originalIdentity.dateOfExpiry().getCharValAsString());
        identity.address().putCharVal(
            originalIdentity.address().getCharValAsString());
        identity.verified().putCharVal(verificationResult ? "true" : "false");
    } catch (const std::exception &e) {
        qLogger::get().error_fast("Error sending response: {}", e.what());
    }
    return buffer;
}
