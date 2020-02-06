#ifndef CONNECTION_STATE_HPP_
#define CONNECTION_STATE_HPP_

namespace dib {
enum class ConnectionState : bool
{
  kConnected = true,
  kDisconnected = false
};
}

#endif // CONNECTION_STATE_HPP_
