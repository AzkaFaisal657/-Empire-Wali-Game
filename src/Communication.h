#pragma once
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <vector>
#include <string>
#include <ctime>
#include <iostream>
#include <iomanip>
#include "KingdomManager.h"

// Represents a message between kingdoms
struct Message {
    int messageId;
    int fromKingdomId;
    int toKingdomId;
    std::string content;
    std::time_t timestamp;
    bool isRead;

    Message(int id, int from, int to, const std::string& msg)
        : messageId(id), fromKingdomId(from), toKingdomId(to), content(msg), isRead(false) {
        timestamp = std::time(nullptr);
    }
};

// Communication system for inter-kingdom messaging
class Communication {
private:
    std::vector<Message> messages;
    int nextMessageId;
    KingdomManager& kingdomManager;

public:
    Communication(KingdomManager& km) : kingdomManager(km), nextMessageId(1) {}

    // Send a message from one kingdom to another
    bool sendMessage(int fromKingdomId, int toKingdomId, const std::string& content) {
        // Validate kingdom IDs
        if (!kingdomManager.getKingdomById(fromKingdomId) ||
            !kingdomManager.getKingdomById(toKingdomId)) {
            return false;
        }

        // Create and store the message
        messages.emplace_back(nextMessageId++, fromKingdomId, toKingdomId, content);
        return true;
    }

    // Get all messages for a kingdom (both sent and received)
    std::vector<const Message*> getAllMessagesForKingdom(int kingdomId) const {
        std::vector<const Message*> result;
        for (const auto& msg : messages) {
            if (msg.fromKingdomId == kingdomId || msg.toKingdomId == kingdomId) {
                result.push_back(&msg);
            }
        }
        return result;
    }

    // Get unread messages for a kingdom
    std::vector<const Message*> getUnreadMessagesForKingdom(int kingdomId) const {
        std::vector<const Message*> result;
        for (const auto& msg : messages) {
            if (msg.toKingdomId == kingdomId && !msg.isRead) {
                result.push_back(&msg);
            }
        }
        return result;
    }

    // Get received messages for a kingdom
    std::vector<const Message*> getReceivedMessagesForKingdom(int kingdomId) const {
        std::vector<const Message*> result;
        for (const auto& msg : messages) {
            if (msg.toKingdomId == kingdomId) {
                result.push_back(&msg);
            }
        }
        return result;
    }

    // Get sent messages from a kingdom
    std::vector<const Message*> getSentMessagesForKingdom(int kingdomId) const {
        std::vector<const Message*> result;
        for (const auto& msg : messages) {
            if (msg.fromKingdomId == kingdomId) {
                result.push_back(&msg);
            }
        }
        return result;
    }

    // Mark a message as read
    bool markMessageAsRead(int messageId) {
        for (auto& msg : messages) {
            if (msg.messageId == messageId) {
                msg.isRead = true;
                return true;
            }
        }
        return false;
    }

    // Mark all messages for a kingdom as read
    void markAllMessagesAsRead(int kingdomId) {
        for (auto& msg : messages) {
            if (msg.toKingdomId == kingdomId) {
                msg.isRead = true;
            }
        }
    }

    // Get a specific message by ID
    const Message* getMessageById(int messageId) const {
        for (const auto& msg : messages) {
            if (msg.messageId == messageId) {
                return &msg;
            }
        }
        return nullptr;
    }

    // Delete a message (only if you're the sender or recipient)
    bool deleteMessage(int messageId, int kingdomId) {
        for (auto it = messages.begin(); it != messages.end(); ++it) {
            if (it->messageId == messageId &&
                (it->fromKingdomId == kingdomId || it->toKingdomId == kingdomId)) {
                messages.erase(it);
                return true;
            }
        }
        return false;
    }

    // Display inbox for a kingdom
    void displayInbox(int kingdomId) const {
        Kingdom* kingdom = kingdomManager.getKingdomById(kingdomId);
        if (!kingdom) return;

        auto receivedMessages = getReceivedMessagesForKingdom(kingdomId);

        std::cout << "\n===== INBOX FOR " << kingdom->getName() << " =====" << std::endl;

        if (receivedMessages.empty()) {
            std::cout << "No messages received." << std::endl;
        }
        else {
            std::cout << std::left << std::setw(5) << "ID"
                << std::setw(20) << "From"
                << std::setw(20) << "Date"
                << std::setw(10) << "Status"
                << "Message" << std::endl;
            std::cout << "-----------------------------------------------------------------------" << std::endl;

            for (const auto* msg : receivedMessages) {
                Kingdom* sender = kingdomManager.getKingdomById(msg->fromKingdomId);
                std::string senderName = sender ? sender->getName() : "Unknown";

                std::cout << std::left << std::setw(5) << msg->messageId
                    << std::setw(20) << senderName
                    << std::setw(10) << (msg->isRead ? "Read" : "Unread")
                    << (msg->content.length() > 50 ? msg->content.substr(0, 47) + "..." : msg->content)
                    << std::endl;
            }
        }
        std::cout << "===============================" << std::endl;
    }

    // Display outbox for a kingdom
    void displayOutbox(int kingdomId) const {
        Kingdom* kingdom = kingdomManager.getKingdomById(kingdomId);
        if (!kingdom) return;

        auto sentMessages = getSentMessagesForKingdom(kingdomId);

        std::cout << "\n===== OUTBOX FOR " << kingdom->getName() << " =====" << std::endl;

        if (sentMessages.empty()) {
            std::cout << "No messages sent." << std::endl;
        }
        else {
            std::cout << std::left << std::setw(5) << "ID"
                << std::setw(20) << "To"
                << std::setw(20) << "Date"
                << std::setw(10) << "Status"
                << "Message" << std::endl;
            std::cout << "-----------------------------------------------------------------------" << std::endl;

            for (const auto* msg : sentMessages) {
                Kingdom* recipient = kingdomManager.getKingdomById(msg->toKingdomId);
                std::string recipientName = recipient ? recipient->getName() : "Unknown";

                std::cout << std::left << std::setw(5) << msg->messageId
                    << std::setw(20) << recipientName
                    << std::setw(10) << (msg->isRead ? "Read" : "Unread")
                    << (msg->content.length() > 50 ? msg->content.substr(0, 47) + "..." : msg->content)
                    << std::endl;
            }
        }
        std::cout << "===============================" << std::endl;
    }

    // Display a full message
    void displayMessage(int messageId) const {
        const Message* msg = getMessageById(messageId);
        if (!msg) {
            std::cout << "Message not found." << std::endl;
            return;
        }

        Kingdom* sender = kingdomManager.getKingdomById(msg->fromKingdomId);
        Kingdom* recipient = kingdomManager.getKingdomById(msg->toKingdomId);

        std::string senderName = sender ? sender->getName() : "Unknown";
        std::string recipientName = recipient ? recipient->getName() : "Unknown";

        std::cout << "\n===== MESSAGE #" << msg->messageId << " =====" << std::endl;
        std::cout << "From: " << senderName << std::endl;
        std::cout << "To: " << recipientName << std::endl;
        std::cout << "Status: " << (msg->isRead ? "Read" : "Unread") << std::endl;
        std::cout << "-----------------------------------------------------------------------" << std::endl;
        std::cout << msg->content << std::endl;
        std::cout << "===============================" << std::endl;
    }
};

#endif // COMMUNICATION_H
