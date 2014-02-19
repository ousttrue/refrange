mpack
=====
Msgpackを自分用に再実装する

要件
----
* headerのincludeのみで動くようにする。([msgpack-cpp-lite](https://code.google.com/p/msgpack-cpp-lite/)と被るのではないか)
* デシリアライズをせずにトラバースするapiを提供する
* シリアライズをせずに動的に構築するapiを提供する
* 型のあるBoost.PropertyTreeみたいな使い勝手のapiを提供する
* JSONと相互に変換するapi
* シンプル実装を心がける
* Test Driven

ToDo
----
* packer エラー型
* unpacker エラー型(eof, 数値代入時のoverflow, underflow, 桁落ち)
* sequence型api整理
* 後で[msgpack-rpc-asio](https://github.com/ousttrue/msgpack-rpc-asio)のバックエンドにする
