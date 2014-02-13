mpack
=====
Msgpackを自分用に再実装する

要件
----
* headerのincludeのみで動くようにする。([msgpack-cpp-lite](https://code.google.com/p/msgpack-cpp-lite/)と被るのではないか)
* デシリアライズせずトラバースする便利apiを提供する
* 型のあるBoost.PropertyTreeみたいな使い勝手(効率は気にしないで使い勝手重視)
* JSONと相互に変換する
* 後で[msgpack-rpc-asio](https://github.com/ousttrue/msgpack-rpc-asio)のバックエンドにする
* templateはほどほどにシンプル実装を心がける
* とりあえずvc2010向け

