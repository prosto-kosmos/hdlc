// ---------------------------------------------------------------------------

#include <vcl.h>
#include "cmath"

// переобределяем тип: создем новый тип TCallBackModFunction
typedef void(*TCallBackModFunction)(unsigned char*Data, int LengthOfDataInByte,
	int LengthOfDataInBit, char correct);

class HDLC_CLASS {
private:
	TCallBackModFunction FCallBackFunction;

	unsigned char *Packet;
	int countPacket;
	int countFive;
	int countFullByte;
	unsigned char packet_Byte;
	unsigned char window;
	int i, j, flag_octatka_bloka, delta;
	unsigned char maska[8];
	unsigned char lastByteBlock;
	int flag_begin_packet;

public:
	__fastcall HDLC_CLASS(TCallBackModFunction CallBackFunction) {
		FCallBackFunction = CallBackFunction;

		Packet = new unsigned char[65536];
		memset(Packet, 0, 65536);
		countPacket = 0;
		countFive = 0;
		countFullByte = 0;
		window = 0;
		packet_Byte = 0;
		flag_octatka_bloka = 0;
		delta = 0;

		maska[0] = 0x01;
		maska[1] = 0x02;
		maska[2] = 0x04;
		maska[3] = 0x08;
		maska[4] = 0x10;
		maska[5] = 0x20;
		maska[6] = 0x40;
		maska[7] = 0x80;
	};

	__fastcall ~HDLC_CLASS(void) {
		delete[]Packet;
	};

	inline void __fastcall Add_Bit_in_Packet(unsigned char in_Bit) {
		if (countFive >= 5) {
			countFive = 0;
			return;
		}
		if (in_Bit) {
			countFive++;
			packet_Byte = packet_Byte | maska[countFullByte];
		}
		else {
			countFive = 0;
		}
		countFullByte += 1;
		if (countFullByte == 8) { // если байт сформирован кидаем его в пакет
			Packet[countPacket++] = packet_Byte;
			if (countPacket >= 65536) {
				countPacket = 0;
			}
			packet_Byte = 0;
			countFullByte = 0;
		}
	}

	inline void __fastcall f_0x7e_in_begin(unsigned char *Data,
		int LengthOfData) {
		// ------------------------------------------------------------------
		// если 0х7е попал в стык блоков
		// ------------------------------------------------------------------
		while (window == 0x7e) {
			if (!flag_begin_packet) {
				window = 0xff & (lastByteBlock << (delta + 1));
				window = window | (Data[i] >> (8 - delta + 1));
				flag_begin_packet = 1;
				if (window == 0x7e)
					continue;
				else
					flag_begin_packet = 2;
			}
			else {
				window = 0xff & (Data[i] << (delta + 1));
				window = window | (Data[i + 1] >> (8 - delta + 1));
			}
			if (window == 0x7e)
				i++;
			else {
				flag_octatka_bloka = (j + 1) % 8;
				window = 0;
				break;
			}
		}
		if (flag_begin_packet == 2) {
			for (j = flag_octatka_bloka; j < 8; j++) {
				window = window >> 1;
				window = window | (lastByteBlock & maska[j]) << (7 - j);
				Add_Bit_in_Packet(lastByteBlock & maska[j]);
			}
			flag_octatka_bloka = 0;
			flag_begin_packet = 1;
		}
		// ------------------------------------------------------------------
		// ------------------------------------------------------------------
	}

	void __fastcall AddData(unsigned char *Data, int LengthOfData) {
		flag_begin_packet = 0;
		for (i = 0; i < LengthOfData; i++) {
			if (window == 0x7e) {
				f_0x7e_in_begin(Data, LengthOfData);
			}
			for (j = 0; j < 8; j++) {
				window = window >> 1; // освободили верхний бит окна
				window = window | (Data[i] & maska[j]) << (7 - j); // сняли очередной бит с входного блока и положили его наверх окна
				Add_Bit_in_Packet(Data[i] & maska[j]); // очередной бит с входного блока добавляем в пакет
				if (window == 0x7e) {
					if (countPacket != 0) { // если 0х7е - завершает пакет, то отправляем пакет в миллер
						FCallBackFunction(Packet, countPacket, countPacket * 8,
							0);
					}
					memset(Packet, 0, countPacket);
					countPacket = 0;
					packet_Byte = 0;
					countFive = 0;
					countFullByte = 0;

					while (1) { // находимся в цикце, пока не закончится последовательность 0х7е
						if (i == (LengthOfData - 1)) {
							lastByteBlock = Data[LengthOfData - 1];
							return;
						}
						delta = j; // дальше будем двигаться с шагом 8 бит
						window = 0xff & (Data[i] << (delta + 1));
						window = window | (Data[i + 1] >> (8 - delta + 1));
						if (window == 0x7e)
							i++;
						else { // закончилась последовательность подрядидущих 0х7е
							window = 0;
							break;
						}
					}
				}
			}
		}
	}
};
// ---------------------------------------------------------------------------
