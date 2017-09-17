// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifndef COMMON_ATLAS_STREAM_CLIENT_H
#define COMMON_ATLAS_STREAM_CLIENT_H

#include "common/OperationRouter.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>

#include <boost/asio.hpp>

#include <chrono>
#include <functional>
#include <list>

namespace Atlas {
  class Codec;
} // namespace Atlas

class ClientTask;

class StreamClientSocketBase
{
    public:
        StreamClientSocketBase(boost::asio::io_service& io_service, std::function<void()>& dispatcher);
        virtual ~StreamClientSocketBase();

        int negotiate(Atlas::Objects::ObjectsDecoder& decoder);
        std::iostream& getIos();

        Atlas::Codec& getCodec();
        Atlas::Objects::ObjectsEncoder& getEncoder();

        virtual size_t write() = 0;
        int poll(const boost::posix_time::time_duration& duration);
        int poll(const boost::posix_time::time_duration& duration, const std::function<bool()> exitCheckerFn);
    protected:
        enum
        {
            read_buffer_size = 16384
        };
        boost::asio::io_service& m_io_service;
        std::function<void()> mDispatcher;

        boost::asio::streambuf mBuffer;
        boost::asio::streambuf mReadBuffer;
        std::iostream m_ios;
        Atlas::Codec* m_codec;
        Atlas::Objects::ObjectsEncoder * m_encoder;
        bool m_is_connected;

        virtual size_t read_blocking() = 0;
        virtual void do_read() = 0;
};

class TcpStreamClientSocket : public StreamClientSocketBase
{
    public:
        TcpStreamClientSocket(boost::asio::io_service& io_service, std::function<void()>& dispatcher, boost::asio::ip::tcp::endpoint endpoint);
        size_t write() override;
   protected:
        boost::asio::ip::tcp::socket m_socket;
        size_t read_blocking() override;
        void do_read() override;
};

class LocalStreamClientSocket : public StreamClientSocketBase
{
    public:
        LocalStreamClientSocket(boost::asio::io_service& io_service, std::function<void()>& dispatcher, boost::asio::local::stream_protocol::endpoint endpoint);
        size_t write() override;
    protected:
        boost::asio::local::stream_protocol::socket m_socket;
        size_t read_blocking() override;
        void do_read() override;
};


class AtlasStreamClient : public Atlas::Objects::ObjectsDecoder
{
  protected:
    boost::asio::io_service m_io_service;
    /// \brief Use a "work" instance to make sure the io_service never runs out of work and is stopped.
    boost::asio::io_service::work m_io_work;

    /// \brief Flag to indicate that a reply has been received from the server
    bool reply_flag;
    /// \brief Flag to indicate that an error has been received from the server
    bool error_flag;
    /// \brief Counter used to track serial numbers sent to the server
    int serialNo;

    StreamClientSocketBase* m_socket;
    ClientTask * m_currentTask;

    std::string m_username;
    size_t m_spacing;

    /// \brief Store for reply data from the server
    Atlas::Objects::Root m_infoReply;

    /// \brief Account identifier returned after successful login
    std::string m_accountId;
    /// \brief Account type returned after login
    std::string m_accountType;
    /// \brief Stored error message from the last received Error operation
    std::string m_errorMessage;

    std::list<Atlas::Objects::Operation::RootOperation> mOps;

    // void objectArrived(const Atlas::Objects::Root &);
    int waitForLoginResponse();
    void dispatch();

    virtual void objectArrived(const Atlas::Objects::Root &);

    virtual void operation(const Atlas::Objects::Operation::RootOperation &);

    virtual void infoArrived(const Atlas::Objects::Operation::RootOperation &);
    virtual void errorArrived(const Atlas::Objects::Operation::RootOperation &);
    virtual void appearanceArrived(const Operation &);
    virtual void disappearanceArrived(const Operation &);
    virtual void sightArrived(const Operation &);
    virtual void soundArrived(const Operation &);

    virtual void loginSuccess(const Atlas::Objects::Root & arg);

  public:
    AtlasStreamClient();
    virtual ~AtlasStreamClient();

    int newSerialNo() {
        return ++serialNo;
    }

    const Atlas::Objects::Root & getInfoReply() const {
        return m_infoReply;
    }

    const std::string & errorMessage() const {
        return m_errorMessage;
    }

    size_t spacing() const {
        return m_spacing;
    }

    virtual void send(const Atlas::Objects::Operation::RootOperation & op);
    int connect(const std::string & host, unsigned short port = 6767);
    int connectLocal(const std::string & host);
    int cleanDisconnect();
    int login(const std::string & username, const std::string & password);
    int create(const std::string & type,
               const std::string & username,
               const std::string & password);
    int pollOne(const boost::posix_time::time_duration& duration);
    int poll(const boost::posix_time::time_duration& duration);
    int poll(int seconds = 0, int microseconds = 0);
    void output(const Atlas::Message::Element & item, size_t depth = 0) const;
    void output(const Atlas::Objects::Root & item) const;

    int runTask(ClientTask * task, const std::string & arg);
    int endTask();

    /**
     * Checks if there's an active task.
     * @return True if there's a task set.
     */
    bool hasTask() const;

    /**
     * Poll the server until the current task has completed.
     * @return 0 if successful
     */
    int pollUntilTaskComplete();

};

#endif // COMMON_ATLAS_STREAM_CLIENT_H
