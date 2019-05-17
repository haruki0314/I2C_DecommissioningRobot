//////////////////////////////////////////////////////////////////https://qiita.com/iigura/items/034ef2fcf12bdb57fd63
//0　i2cdetect -y 1　　←を実行するとaruduinoがI2Cデバイスとして機能しているか確認できる。
//cd wiringPi←開く 
//１　sudo ./a.out ←　プログラムの実行
//２　アドレスが要求されるから0でみたアドレスを打つ
//３　すきな文字をうつ（俺らの場合6bite送った　例：12beef）
//arduino側に情報（6bite）が送られる



//WiringPi を用いた Raspberry Pi 側のプログラム
//まずは 3 バイトを送るサンプルプログラム
//Raspberry Pi から I2C 経由でデータを送信するために、WiringPi を使用します。
//このライブラリには、wiringPiI2CWriteReg16 関数などがあり、一見するとレジスタ番号を含めて最大 3 バイトしか送信できないの？
//と思ってしまいます（私も誤解していました）。
//実は、wiringPiI2CSetup() の戻り値は標準的な Linux のファイルハンドルです。
//そのため、これを用いて、read() や write() などの関数を使用し、複数バイトのデータも送受信可能です（任意の複数バイトからなるデータを送信する
//プログラムを List3 として後ほど示します）。
///
//List 2 では、wiringPiI2CWriteReg16() を使って、ラズパイから Arduino へレジスタ番号を含め、3 バイトのデータを送信してみます。Raspbery Pi //側のプログラムはスレイブとして動作しているターゲットの I2C アドレス（List1 の場合は 0x04）や、WiringPi 側の要請で
//レジスタ番号を入力するようにしています。もちろん、16 ビットの送信データも入力できるようにしています。入力するデータは全て 16 進数とします。
//頭に 0x という文字は不要です。例えば、12 と入力したら、それは 0x12 として解釈されます。
//
//
//
//
//実行方法　　　g++ i2cMasterTest.cpp -+wiringi　　としてターミナルでコンパイルする。すると a.out ができるので
//　　sudo ./a.out　として実行 その後ターゲットに04　レジスタに適当な文字or値をうち、データにbeefとして入力すると　Arduino IDEシリアルモニタ
//　　　　　　　　　　には　　　NumOfBytes=3 : 12 BE EF　と表示されるはず。
//
//
//
//////////////////////////////////////////////////////
// I2C maste test
// g++ i2cMasterTest.cpp -lwiringPi

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

int main() {
    if(wiringPiSetup()<0) {  // WiringPi が初期化できなかったらエラー表示して終了
        fprintf(stderr,"Error: can not init wiringPi.\n");
        fprintf(stderr,"errno=%d\n",errno);
        fprintf(stderr,"%s\n",strerror(errno));
        return -1;
    }

    // スレーブ側の I2C アドレスの入力
    int addr;
    printf("input target addr: ");
    scanf("%02x",&addr);  // とりあえず…
    printf("TARGET Addr=0x%02X[=%d]\n",addr,addr);

    // 対象の I2C デバイスへの通信路を開く
    int fd=wiringPiI2CSetup(addr);
    if(fd<0) {
        fprintf(stderr,"ERROR: "  // 長いので 2 行に分けた
                       "Can not start to communicate I2C device %0x02X.\n",addr);
        return -1;
    }

    // 対象のレジスタ（List1 にはそんなものは無いが送信のために必要なので…）の入力
    // ここで入力したバイト値も、次に入力する 16 ビット値の前に送信される。
    int reg;
    printf("input target reg: ");
    scanf("%02x",&reg);
    printf("TARGET Reg=0x%02X[=%d]\n",reg,reg);

    // 送信するデータ
    int data;
    printf("input send data (16bit): ");
    scanf("%04x",&data);
    printf("SEND Data=0x%04X[=%d]\n",data,data);
    // エンディアンの問題で、実際に送信してみると上位バイトと下位バイトが入れ替わるので
    // 送信前に入れ替えておく（0x1234 を送ってみると 0x34 0x12 と受信されてしまうので）。
    data=(data&0xFF)*0x100+((data>>8)&0xFF);

    // 送信！
    int result=wiringPiI2CWriteReg16(fd,reg,data);
    if(result<0) {
        fprintf(stderr,"Error: can not write to the target.\n");
        fprintf(stderr,"errno=%d\n",errno);
        fprintf(stderr,"%s\n",strerror(errno));
        return -1;
    }

    return 0;
}
