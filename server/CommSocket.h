// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef SERVER_COMM_SOCKET_H
#define SERVER_COMM_SOCKET_H

class CommServer;

/// \brief Base class for all classes for handling socket communication.
class CommSocket {
  protected:
    explicit CommSocket(CommServer & svr);
  public:
    /// Reference to the object that manages all socket communication.
    CommServer & m_commServer;

    virtual ~CommSocket();

    /// \brief Get the socket file descriptor.
    ///
    /// Used to determine if this socket required attention using select().
    virtual int getFd() const = 0;

    /// \brief Determine whether this socket is open.
    /// @return true if the socket conneciton is open, false otherwise.
    virtual bool isOpen() const = 0;

    /// \brief Determine whether this socket has hung up.
    /// @return true if the socket conneciton has hung up, false otherwise.
    virtual bool eof() = 0;

    /// \brief Read date from the socket.
    ///
    /// Called when select has determined that this socket requires attention.
    /// This function should read data from the socket and do any initial
    /// processing required, but must not write to the socket. Writing to
    /// to the socket during this function could result in a timeout
    /// which could result in the server locking up. An processing of the
    /// incoming date which might result in a write to the socket must
    /// be performed in the dispatch() memeber function.
    /// @return true if some exceptional circumstance occured meaning this
    /// socket is no longer usable, false otherwise.
    virtual int read() = 0;

    /// \brief Post process data read from the socket.
    ///
    /// Called after read(), this function should perform post any processing
    /// or dispatch on data that has been read which might result in
    /// writing to the client. A typical example is an Atlas operation
    /// from the client which might have a reply. The Atlas decode is performed
    /// in read(), but the dispatch is delayed until dispatch().
    virtual void dispatch() = 0;
};

#endif // SERVER_COMM_SOCKET_H
