//ref: IK13::Tick()	mk72r-code-r104\sources\core\mk61emu.cpp

#include "mk61emu_core.h"
#define dbgln(...)

//extern int MOD42(int);
//extern int DIV3 (int);
#define DIV3(v)		(((v)*171)>>9)
//#define MOD42(v)	( (v)-(((v)*49)>>11)*42 ) 	//	//valid for x range [0..208]

#define MUL9(v)		((v)*9)
#define MOD42(v)	( ((v) %42) &0xff )

static const uint8_t IK1302_DCW[68] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x02, 0x00, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02,
  0x03, 0x00, 0x00, 0x00
};

static const uint8_t IK1302_DCWA[68] = {
  0x00, 0x02, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x00, 0x02, 0x00, 0x10, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
  0x02, 0x02, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0A, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x02,
  0x02, 0x08, 0x08, 0x00, 0x0E, 0x00, 0x00, 0x08, 0x02, 0x04, 0x08, 0x02, 0x08, 0x02, 0x06, 0x0C,
  0x04, 0x04, 0x00, 0x0A, 0x0C, 0x02, 0x00, 0x02, 0x02, 0x02, 0x0C, 0x02, 0x02, 0x0C, 0x02, 0x02,
  0x00, 0x00, 0x02, 0x02
};

inline __attribute__((always_inline))
void mi_exec(IK1302& dev, mtick_t signal_I, const microinstruction_t mi) {
	uint32_t mi_hi, val;
	uint32_t microinstruction = mi;
    uint32_t tmp = dev.AMK;

    mi_hi = (microinstruction >> 16);
//---------------------------------------------------------
    if((((microinstruction >> 24) & 0x03) == 0x2) || (((microinstruction >> 24) & 0x03) == 0x3)) {
        if (DIV3(signal_I) != dev.key_xm)
           dev.S1 |= dev.key_y;
    }
//---------------------------------------------------------
    io_t alpha = 0;
    io_t gamma = 0;
    io_t sigma = 0;

    if((microinstruction & 0x7FFF) != 0){	//	microinstruction&0x7f => 01:30,00:13,20:12,08:5,02:3,10:2,40:1,04:1,09:1 
        switch(IK1302_DCWA[tmp/*dev.AMK*/]) {
                case 0: alpha = 0; break;
                case 0x0002: alpha = dev.R[signal_I]; break;
                case 0x0004: alpha = dev.pM[signal_I]; break;
                case 0x0006: alpha = dev.ST[signal_I]; break;
                case 0x0008: alpha = ~dev.R[signal_I] & 0xf; break;
                case 0x000A: if (dev.L == 0) alpha = 0xa; else alpha = 0; break;
                case 0x000C: alpha = dev.S; break;
                case 0x000E: alpha = 4; break;
                case 0x0010: alpha = 0xf;
        }

        if((microinstruction & 0x0F80) != 0) {	//	9 0000:27,0800:12,0080:12,0200:5,0180:4,0100:3,0400:2,0c00:2,0280:1
          switch(microinstruction & 0x0F80) {
                case 0x0800: alpha += 1; break;
                case 0x0400: alpha += 6; break;
                case 0x0C00: alpha += (1|6); break;
                case 0x0080: alpha += dev.S; break;
                case 0x0100: alpha += (~dev.S & 0xf); break;
                case 0x0200: alpha += dev.S1; break;
                case 0x0180: alpha += 0xf; break;
                case 0x0280: alpha += (dev.S | dev.S1); break;
          }
     }

   //---------------------------------------------------------
        if (dev.flag_FC > 0){
                if (dev.key_y == 0) dev.T = 0;
        } else {
                dev.displayed = 1;
                val = DIV3(signal_I);
                if((val == dev.key_xm) && (dev.key_y != 0)){
                                dev.S1 = dev.key_y;
                                dev.T = 1;
                }
                if((val < 12) && (dev.L != 0)) dev.comma = val;
        }
   //---------------------------------------------------------
        if((microinstruction & 0x4000) != 0) gamma = dev.T ^ 1; else gamma = 0;
        if((microinstruction & 0x2000) != 0) gamma |= dev.L ^ 1;
        if((microinstruction & 0x1000) != 0) gamma |= dev.L;

        alpha += gamma;
        sigma = alpha & 0xf;
        dev.P = alpha >> 4;
    } else {
   //---------------------------------------------------------
        if (dev.flag_FC > 0){
                if (dev.key_y == 0) dev.T = 0;
        } else {
                dev.displayed = 1;
                val = DIV3(signal_I);
                if((val == dev.key_xm) && (dev.key_y != 0)){
                                dev.S1 = dev.key_y;
                                dev.T = 1;
                }
                if((val < 12) && (dev.L != 0)) dev.comma = val;
        }
    //---------------------------------------------------------
        sigma = 0;
        dev.P = 0;
    }
#ifdef out_dump
printf("AMK %4.4X, microinstruction: %8.8X, MOD %u, S %u, S1 %u, sigma %u\n", dev.AMK, microinstruction, IK1302_MOD, IK1302_S, IK1302_S1, sigma);
#endif
//---------------------------------------------------------
    if (dev.MOD == 0 || signal_I >= 36) {
        tmp = IK1302_DCW[tmp/*dev.AMK*/];
        if(tmp != 0){
          switch (tmp) {	// page #109
            case 1: 
                dbgln(CORE61, "R[", signal_I, "] = R[", MOD42(signal_I + 3));
                dev.R[signal_I] = dev.R[MOD42(signal_I + 3)]; 
              break;
            case 2: dev.R[signal_I] = sigma; break;
            case 3: dev.R[signal_I] = dev.S; break;
            case 4: dev.R[signal_I] = dev.R[signal_I] | dev.S | sigma; break;
            case 5: dev.R[signal_I] = dev.S | sigma; break;
            case 6: dev.R[signal_I] = dev.R[signal_I] | dev.S; break;
            case 7: dev.R[signal_I] = dev.R[signal_I] | sigma;
          }
        }

        if ((mi_hi & 0x0004) !=0)    dev.R[MOD42(signal_I + 41)] = sigma;
        if ((mi_hi & 0x0008) !=0)    dev.R[MOD42(signal_I + 40)] = sigma;
    }
    if ((mi_hi & 0x0020) !=0)        dev.L = dev.P & 1;
    if ((mi_hi & 0x0010) !=0)        dev.pM[signal_I] = dev.S;
//---------------------------------------------------------

        if((mi_hi & 0x0040) == 0){
         // 6 bit == 0, ����� ���� 7 ��� �� �����?
          if((mi_hi & 0x0080) != 0)  dev.S = sigma; // 7 bit == 1
        } else {
         // 6 bit == 1, ����� ���� 7 ��� ���� �����?
                dev.S = dev.S1;
          if((mi_hi & 0x0080) != 0) dev.S |= sigma;
        }

        // 6 bit == 0, ��� ����� �� ��������� 7 ����
        if((mi_hi & 0x0100) != 0){
         // 6 bit == 1, ����� ���� 7 ��� ���� �����?
          if((mi_hi & 0x0200) != 0) dev.S1 |= sigma; else dev.S1 = sigma;
        }
//----------------------------------------------------------
    mi_hi = mi_hi & 0x0C00;
    if(mi_hi != 0){
          if(mi_hi == 0x0400) {
                    dev.ST[MOD42(signal_I + 2)] = dev.ST[MOD42(signal_I + 1)];
                    dev.ST[MOD42(signal_I + 1)] = dev.ST[signal_I];
                    dev.ST[signal_I] = sigma;
          } else if(mi_hi == 0x0800) {
                    tmp = dev.ST[signal_I];
                    dev.ST[signal_I] = dev.ST[MOD42(signal_I + 1)];
                    dev.ST[MOD42(signal_I + 1)] = dev.ST[MOD42(signal_I + 2)];
                    dev.ST[MOD42(signal_I + 2)] = tmp;
          }
    }
}
