# my7seg
## 概要
my7segは名前の通り7セグメントLEDを制御するデバイスドライバである．\
動作動画　https://youtu.be/oA6VUcjMmJ4
## 環境
* ハードウェア\
Raspberry Pi 4 Model B
* ソフトウェア
  * OS\
  Ubuntu20.04
  * コンパイラ\
  gcc version 9.3.0

## 使用する7セグメントについて
今回はカソードコモンの7セグメントLEDを使用する．

## 仕様
* /dev/my7seg0　にechoで数字かA~Fのアルファベット（小文字も可）を入れると7セグメントLEDに表示する．改行とスペースは無視してそれ以外の文字はドットを表示する．
* cat で/dev/my7seg0 を表示すると最後に入力した文字が表示される．（改行とスペースを除く）

## 準備
1. このリポジトリのクローンを作成する\
`git clone https://github.com/Akutagawa50/my7seg.git`
<br>

2. my7segに移動する\
`cd my7seg`
<br>

3. makeコマンドを実行する\
`make`
<br>

4. カーネルモジュールをインストールする\
`sudo insmod my7seg.ko`\
このときLEDが全消灯する

5. できたデバイスドライバに権限を付与して準備完了\
`sudo chmod 666 /dev/my7seg0`
<br>

その他のコマンド
* ログを見たい場合\
`tail /var/log/kern.log`
<br>

* カーネルモジュールをアンインストールする場合\
`sudo rmmod my7seg`
<br>

## 使い方
* LEDに出力するとき
aを出力するとき\
`echo a >> /dev/my7seg0`
<br>

* 最後に入力した文字を確認するとき\
`cat /dev/my7seg0`\
出力結果\
`a`

## ライセンス
このリポジトリはGPLv3が付与されています．\
詳しくはこちらから [COPYING](https://github.com/Akutagawa50/my7seg/blob/main/COPYING)

