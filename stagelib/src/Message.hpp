// StageLib - Message.hpp
// Copyright (c) 2021 Alistair Buxton <a.j.buxton@gmail.com>

#pragma once

#include <cstdint>
#include <cstring>
#include <variant>

#include "32blit.hpp"

enum class UserMessage : uint8_t; // Must be fully defined in user code.

#pragma pack(push, 1)
class Message {
    // --- enums ------------------------------------------------------
    public:

        enum class ErrorState : uint8_t {
            None = 0,
            PeerMismatch = 1,
            PeerTooOld = 2,
            PeerTooNew = 3,
            PeerSignal = 4,
            OutOfSeqUserMsg = 5,
            UnknownSysMsg = 6,
            Disconnected = 7,
        };

        enum class PairingState {
            Idle = 0,
            Error = 1,
            Pairing = 2,
            Paired = 3,
        };

    private:
        enum class SystemMessage : uint8_t {
            User = 0,
            PairReq = 1,
            PairAck = 2,
            PairEnd = 3,
            Error = 4
        };

    // --- constructors -----------------------------------------------
    public:
        explicit Message(UserMessage t) : _sys(SystemMessage::User), _usr(t) {}

    private:
        explicit Message(SystemMessage s) : _sys(s), _usr(UserMessage()) {}
        explicit Message(ErrorState e) : _sys(SystemMessage::Error), _err(e) {}

    // --- receiver methods -------------------------------------------

    public:
        [[nodiscard]] ErrorState validate() const {
            if (_pub != publisher || _app != application) {
                return ErrorState::PeerMismatch;
            }
            if (_ver != version) {
                return (_ver > version) ? ErrorState::PeerTooNew : ErrorState::PeerTooOld;
            }
            return ErrorState::None;
        }

        [[nodiscard]] UserMessage user() const { return _usr; }

        [[nodiscard]] const void *data() const { return this + 1; }
        template<typename T> [[nodiscard]] T &data() const { return *(T *) data(); }

    // --- sender methods ---------------------------------------------

    public:
        void send(uint16_t data_len) { blit::send_message((uint8_t *) this, sizeof(*this) + data_len); }

        // Convenience function for sending a UserMessage with no data.
        static void send(UserMessage t) { Message msg(t); msg.send(0); };

        // Convenience function for sending a UserMessage with a single data item.
        template<typename T>
        static void send(UserMessage t, T data) {
            uint8_t buffer[sizeof(Message) + sizeof(T)];
            auto *msg = new(buffer) Message(t);
            msg->data<T>() = data;
            msg->send(sizeof(T));
        }

        // Convenience function for sending a UserMessage with an array of N items.
        template<typename T, int N>
        static void send(UserMessage t, const T *data) {
            uint8_t buffer[sizeof(Message) + (sizeof(T) * N)];
            auto *msg = new(buffer) Message(t);
            memcpy((void *)msg->data(), data, sizeof(T) * N);
            msg->send(sizeof(T) * N);
        }

    private:

        // Convenience function for sending a SystemMessage::Error.
        static void send(ErrorState e) {
            Message msg(e);
            msg.send(0);
        }

        // Convenience function for sending a SystemMessage with no data.
        static void send(SystemMessage s) {
            Message msg(s);
            msg.send(0);
        }

        // Convenience function for sending a SystemMessage with a single data item.
        template<typename T>
        static void send(SystemMessage s, T data) {
            uint8_t buffer[sizeof(Message) + sizeof(T)];
            auto *msg = new(buffer) Message(s);
            msg->data<T>() = data;
            msg->send(sizeof(T));
        }

    // --- Instance fields ---------------------

    private:

        // These have to be defined in user code.
        static const uint16_t publisher;
        static const uint16_t application;
        static const uint16_t version;

        // The actual fields that get sent over the network:
        const uint16_t _pub = publisher;
        const uint16_t _app = application;
        const uint16_t _ver = version;
        const SystemMessage _sys;
        union {
            const UserMessage _usr;
            const ErrorState _err;
        };

    // --- Pairing handler --------------------

    public:

        static bool paired() { return pairing_state == PairingState::Paired; }
        static bool arbiter() { return _arbiter; }

        static ErrorState get_last_error() { return last_error; }

        static void pair() {
            if (pairing_state == PairingState::Error) unpair();
            if (pairing_state == PairingState::Idle) {
                pairing_state = PairingState::Pairing;
                initiative = blit::random();
                send<uint32_t>(SystemMessage::PairReq, initiative);
            }
        }

        static void unpair() {
            send(SystemMessage::PairEnd);
            pairing_state = PairingState::Idle;
            last_error = ErrorState::None;
            peer_error = ErrorState::None;
            initiative = 0;
        }

        bool handle() {
            if(error(validate())) return true;
            switch(_sys) {
                case SystemMessage::PairReq:
                    if (initiative < data<uint32_t>()) {
                        pairing_state = PairingState::Paired;
                        _arbiter = true;
                        send(SystemMessage::PairAck);
                    } else if (initiative == data<uint32_t>()) {
#ifndef NDEBUG
                        printf("Warning: Initiative collision.\n");
#endif
                        initiative = blit::random();
                        send<uint32_t>(SystemMessage::PairReq, initiative);
                    }
                    return true;
                case SystemMessage::PairAck:
                    pairing_state = PairingState::Paired;
                    _arbiter = false;
                    return true;
                case SystemMessage::PairEnd:
                    pairing_state = PairingState::Idle;
                    initiative = 0;
                    return true;
                case SystemMessage::User:
                    if (!paired()) {
                        return error(ErrorState::OutOfSeqUserMsg);
                    }
                    return false;
                case SystemMessage::Error:
                    last_error = ErrorState::PeerSignal;
                    peer_error = _err;
                    print_error(_err, "Peer Error:");
                    pairing_state = PairingState::Error;
                    return true;
                default:
                    return error(ErrorState::UnknownSysMsg);
            }
        }

    private:

        static bool error(ErrorState e) {
            if (e == ErrorState::None) return false;
            print_error(e, "Error:");
            if (e != last_error) {
                last_error = e;
                if (e != ErrorState::PeerMismatch) {
                    send(e);
                }
                pairing_state = PairingState::Error;
            }
            return true;
        }

        static inline uint32_t initiative = 0;
        static inline bool _arbiter = false;
        static inline ErrorState last_error = ErrorState::None;
        static inline ErrorState peer_error = ErrorState::None;
        static inline PairingState pairing_state = PairingState::Idle;

    // --- Debuging functions -----------------------

    private:

        static void print_error([[maybe_unused]] ErrorState e, [[maybe_unused]] const char *message) {
#ifndef NDEBUG
            if (message) printf("%s ", message);
            switch(e) {
                case ErrorState::None:
                    break;
                case ErrorState::OutOfSeqUserMsg:
                    printf("Illegal UserMessage while unpaired!\n");
                    break;
                case ErrorState::PeerSignal:
                    printf("Peer signalled error!\n");
                    break;
                case ErrorState::PeerTooNew:
                    printf("Peer version is too new!\n");
                    break;
                case ErrorState::PeerTooOld:
                    printf("Peer version is too old!\n");
                    break;
                case ErrorState::PeerMismatch:
                    printf("Peer mismatch!\n");
                    break;
                case ErrorState::Disconnected:
                    printf("Physical disconnect!\n");
                    break;
                case ErrorState::UnknownSysMsg:
                    printf("Unknown system message!\n");
                    break;
            }
#endif
        }



};
#pragma pack(pop)

static_assert(sizeof(Message) == 8, "Message is too big.");


