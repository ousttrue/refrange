refrange
========
Msgpackを自分用に再実装したりする。

要件
----
* headerのincludeのみで動くようにする。([msgpack-cpp-lite](https://code.google.com/p/msgpack-cpp-lite/)と被るのではないか)
* デシリアライズをせずにトラバースするapiを提供する
* シリアライズをせずに動的に構築するapiを提供する
* 型のあるBoost.PropertyTreeみたいな使い勝手のapiを提供する
* JSONと相互に変換するapi
* シンプル実装を心がける
* Test Driven

Text処理
--------

Loader
------

MsgPack
-------

ToDo
----
* refrange::rangeの上にbyte向けとtext向けのreader/writerを実装する
* その上にmsgpack, jsonreader/writerを実装する
* bvh, vpd, vmdのパーサ
* x, mqo, obj, dxf, ply, pmd, pmxのパーサ
* 外部依存無しで動くcore
* boost::asioを使うrefrange_boost_asio 
* boost::anyを使うrefrange_boost_any
* packer エラー型
* unpacker エラー型(eof, 数値代入時のoverflow, underflow, 桁落ち)
* sequence型api整理
* 後で[msgpack-rpc-asio](https://github.com/ousttrue/msgpack-rpc-asio)のバックエンドにする
* boost::string_refとかstd::string_viewみたいの(よくしらない)

