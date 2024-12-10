//ref: IK13::Tick()	mk72r-code-r104\sources\core\mk61emu.cpp

#include "mk61emu_core.h"
#define dbgln(...)

//extern int MOD42(int);
//extern int DIV3 (int);
#define DIV3(v)		(((v)*171)>>9)
//#define MOD42(v)	( (v)-(((v)*49)>>11)*42 ) 	//	//valid for x range [0..208]

#define MUL9(v)		((v)*9)
#define MOD42(v)	( ((v) %42) &0xff )

inline __attribute__((always_inline, hot))
void mi_exec(IK130x_t& dev, mtick_t signal_I, dcw_t& dcw, const microinstruction_t mi) {
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
    if((microinstruction & 0x7FFF) != 0) {
		switch(microinstruction & 0x007F) {
			case 0x0001: alpha = dev.R[signal_I]; break;
			case 0x0002: alpha = dev.pM[signal_I]; break;
			case 0x0004: alpha = dev.ST[signal_I]; break;
			case 0x0005: alpha = dev.ST[signal_I] | dev.R[signal_I]; break;
			case 0x0008: alpha = ~dev.R[signal_I] & 0xf; break;
			case 0x0009: alpha = 0xf; break;
			case 0x0010: if(dev.L == 0) alpha = 0xa; else alpha = 0; break;
			case 0x0020: alpha = dev.S; break;
			case 0x0021: alpha = dev.S | dev.R[signal_I]; break;
			case 0x0028: alpha = dev.S | (~dev.R[signal_I] & 0xf); break;
			case 0x0040: alpha = 4; break;
			case 0: alpha = 0;
		}

		if((microinstruction & 0x0F80) != 0) {
			switch(microinstruction & 0x0F80) {
				case 0x0800: alpha += 1; break;
				case 0x0400: alpha += 6; break;
				case 0x0C00: alpha += (1|6); break;
				case 0x0080: alpha +=  dev.S		; break;
				case 0x0100: alpha += ~dev.S & 0xf	; break;
				case 0x0200: alpha +=  dev.S1		; break;
				case 0x0180: alpha += 0xf			; break;
				case 0x0280: alpha += (dev.S | dev.S1); break;
                case 0x0500: alpha += 6 | (~dev.S & 0xf); break;
			}
		}

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
//#ifdef out_dump
//printf("AMK %4.4X, microinstruction: %8.8X, MOD %u, S %u, S1 %u, sigma %u\n", dev.AMK, microinstruction, IK1302_MOD, IK1302_S, IK1302_S1, sigma);
//#endif
//---------------------------------------------------------
//	auto I1  = MOD42(signal_I +  1);	//slow in x86
//	auto I2  = MOD42(signal_I +  2);
//	auto I3  = MOD42(signal_I +  3);
//	auto I40 = MOD42(signal_I + 40);
//	auto I41 = MOD42(signal_I + 41);

	#define I1  (MOD42(signal_I +  1))
	#define I2  (MOD42(signal_I +  2))
	#define I3  (MOD42(signal_I +  3))
	#define I40 (MOD42(signal_I + 40))
	#define I41 (MOD42(signal_I + 41))


    if (dev.MOD == 0 || signal_I >= 36) {
//        tmp = IK1302_DCW[tmp/*dev.AMK*/];
        tmp = dcw[tmp/*dev.AMK*/];
        if(tmp != 0){
          switch (tmp) {	// page #109
            case 1: 
//                dbgln(CORE61, "R[", signal_I, "] = R[", I3);
                dev.R[signal_I] = dev.R[I3]; 
              break;
            case 2: dev.R[signal_I] = sigma; break;
            case 3: dev.R[signal_I] = dev.S; break;
            case 4: dev.R[signal_I] = dev.R[signal_I] | dev.S | sigma; break;
            case 5: dev.R[signal_I] = dev.S | sigma; break;
            case 6: dev.R[signal_I] = dev.R[signal_I] | dev.S; break;
            case 7: dev.R[signal_I] = dev.R[signal_I] | sigma;
          }
        }

        if ((mi_hi & 0x0004) !=0)    dev.R[I41] = sigma;
        if ((mi_hi & 0x0008) !=0)    dev.R[I40] = sigma;
    }
    if ((mi_hi & 0x0020) !=0)        dev.L = dev.P & 1;
    if ((mi_hi & 0x0010) !=0)        dev.pM[signal_I] = dev.S;
//---------------------------------------------------------

        if((mi_hi & 0x0040) == 0){
         // 6 bit == 0, может быть 7 бит не равен?
          if((mi_hi & 0x0080) != 0)  dev.S = sigma; // 7 bit == 1
        } else {
         // 6 bit == 1, может быть 7 бит тоже равен?
                dev.S = dev.S1;
          if((mi_hi & 0x0080) != 0) dev.S |= sigma;
        }

        // 6 bit == 0, нам пофиг на состояние 7 бита
        if((mi_hi & 0x0100) != 0){
         // 6 bit == 1, может быть 7 бит тоже равен?
          if((mi_hi & 0x0200) != 0) dev.S1 |= sigma; else dev.S1 = sigma;
        }
//----------------------------------------------------------
    mi_hi = mi_hi & 0x0C00;
    if(mi_hi != 0){
          if(mi_hi == 0x0400) {
                    dev.ST[I2]       = dev.ST[I1];
                    dev.ST[I1]       = dev.ST[signal_I];
                    dev.ST[signal_I] = sigma;
          } else if(mi_hi == 0x0800) {
                    tmp = dev.ST[signal_I];
                    dev.ST[signal_I] = dev.ST[I1];
                    dev.ST[I1]       = dev.ST[I2];
                    dev.ST[I2]       = tmp;
          }
    }
}
