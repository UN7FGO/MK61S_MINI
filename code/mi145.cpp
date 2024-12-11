// 145ik13 microinstructions on steroids

#include "mk61emu_core.h"
typedef void (*jtbl_item_t)(IK1302&, mtick_t, int);

extern void mi_exec(IK1302&, mtick_t, const microinstruction_t);	//have to be perfectly inlinable

inline __attribute__((always_inline))
void mi145_exec(IK1302& dev, mtick_t _I, int idx, const jtbl_item_t tbl[]) { tbl[idx](dev, _I, idx); };


#include "mi_exec.hpp"

extern "C" {

__attribute__((weak)) void mi0000000(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00000000);	};	//0
__attribute__((weak)) void mi0008000(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00008000);	};	//1
__attribute__((weak)) void mi0010000(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00010000);	};	//2
__attribute__((weak)) void mi0010020(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00010020);	};	//3
__attribute__((weak)) void mi0010100(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00010100);	};	//4
__attribute__((weak)) void mi0010200(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00010200);	};	//5
__attribute__((weak)) void mi0010801(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00010801);	};	//6
__attribute__((weak)) void mi0011001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00011001);	};	//7
__attribute__((weak)) void mi0011190(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00011190);	};	//8
__attribute__((weak)) void mi0011801(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00011801);	};	//9
__attribute__((weak)) void mi0013081(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00013081);	};	//10
__attribute__((weak)) void mi0013c01(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00013c01);	};	//11
__attribute__((weak)) void mi0040000(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00040000);	};	//12
__attribute__((weak)) void mi0040001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00040001);	};	//13
__attribute__((weak)) void mi0040002(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00040002);	};	//14
__attribute__((weak)) void mi0040020(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00040020);	};	//15
__attribute__((weak)) void mi0040090(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00040090);	};	//16
__attribute__((weak)) void mi0040100(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00040100);	};	//17
__attribute__((weak)) void mi0058001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00058001);	};	//18
__attribute__((weak)) void mi0080001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00080001);	};	//19
__attribute__((weak)) void mi0080020(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00080020);	};	//20
__attribute__((weak)) void mi0080820(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00080820);	};	//21
__attribute__((weak)) void mi0083100(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00083100);	};	//22
__attribute__((weak)) void mi0098001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00098001);	};	//23
__attribute__((weak)) void mi0100000(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00100000);	};	//24
__attribute__((weak)) void mi0140002(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00140002);	};	//25
__attribute__((weak)) void mi0200088(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00200088);	};	//26
__attribute__((weak)) void mi0200090(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00200090);	};	//27
__attribute__((weak)) void mi0200540(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00200540);	};	//28
__attribute__((weak)) void mi0200808(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00200808);	};	//29
__attribute__((weak)) void mi0200809(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00200809);	};	//30
__attribute__((weak)) void mi0201089(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00201089);	};	//31
__attribute__((weak)) void mi0201100(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00201100);	};	//32
__attribute__((weak)) void mi0201420(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00201420);	};	//33
__attribute__((weak)) void mi0203081(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00203081);	};	//34
__attribute__((weak)) void mi0203088(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00203088);	};	//35
__attribute__((weak)) void mi0203090(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00203090);	};	//36
__attribute__((weak)) void mi0203100(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00203100);	};	//37
__attribute__((weak)) void mi0203420(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00203420);	};	//38
__attribute__((weak)) void mi0203c08(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00203c08);	};	//39
__attribute__((weak)) void mi0210081(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00210081);	};	//40
__attribute__((weak)) void mi0210801(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00210801);	};	//41
__attribute__((weak)) void mi0212801(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00212801);	};	//42
__attribute__((weak)) void mi0213201(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00213201);	};	//43
__attribute__((weak)) void mi0213801(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00213801);	};	//44
__attribute__((weak)) void mi0400000(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00400000);	};	//45
__attribute__((weak)) void mi0600081(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00600081);	};	//46
__attribute__((weak)) void mi0601209(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00601209);	};	//47
__attribute__((weak)) void mi0800000(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800000);	};	//48
__attribute__((weak)) void mi0800001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800001);	};	//49
__attribute__((weak)) void mi0800002(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800002);	};	//50
__attribute__((weak)) void mi0800004(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800004);	};	//51
__attribute__((weak)) void mi0800005(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800005);	};	//52
__attribute__((weak)) void mi0800008(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800008);	};	//53
__attribute__((weak)) void mi0800021(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800021);	};	//54
__attribute__((weak)) void mi0800040(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800040);	};	//55
__attribute__((weak)) void mi0800081(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800081);	};	//56
__attribute__((weak)) void mi08000c0(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x008000c0);	};	//57
__attribute__((weak)) void mi0800100(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800100);	};	//58
__attribute__((weak)) void mi0800120(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800120);	};	//59
__attribute__((weak)) void mi0800180(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800180);	};	//60
__attribute__((weak)) void mi08001c0(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x008001c0);	};	//61
__attribute__((weak)) void mi0800280(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800280);	};	//62
__attribute__((weak)) void mi0800400(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800400);	};	//63
__attribute__((weak)) void mi0800401(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800401);	};	//64
__attribute__((weak)) void mi0800420(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800420);	};	//65
__attribute__((weak)) void mi0800800(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800800);	};	//66
__attribute__((weak)) void mi0800801(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800801);	};	//67
__attribute__((weak)) void mi0800820(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800820);	};	//68
__attribute__((weak)) void mi0800840(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00800840);	};	//69
__attribute__((weak)) void mi0801000(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00801000);	};	//70
__attribute__((weak)) void mi0801001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00801001);	};	//71
__attribute__((weak)) void mi0801020(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00801020);	};	//72
__attribute__((weak)) void mi0801190(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00801190);	};	//73
__attribute__((weak)) void mi0803400(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00803400);	};	//74
__attribute__((weak)) void mi0803800(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00803800);	};	//75
__attribute__((weak)) void mi0808001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00808001);	};	//76
__attribute__((weak)) void mi0810002(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00810002);	};	//77
__attribute__((weak)) void mi0810081(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00810081);	};	//78
__attribute__((weak)) void mi0818001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00818001);	};	//79
__attribute__((weak)) void mi0818180(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00818180);	};	//80
__attribute__((weak)) void mi0840020(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00840020);	};	//81
__attribute__((weak)) void mi0840090(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00840090);	};	//82
__attribute__((weak)) void mi0840801(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00840801);	};	//83
__attribute__((weak)) void mi0841020(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00841020);	};	//84
__attribute__((weak)) void mi0858001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00858001);	};	//85
__attribute__((weak)) void mi0880090(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00880090);	};	//86
__attribute__((weak)) void mi0900001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00900001);	};	//87
__attribute__((weak)) void mi0a00028(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a00028);	};	//88
__attribute__((weak)) void mi0a00081(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a00081);	};	//89
__attribute__((weak)) void mi0a00082(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a00082);	};	//90
__attribute__((weak)) void mi0a00088(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a00088);	};	//91
__attribute__((weak)) void mi0a00089(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a00089);	};	//92
__attribute__((weak)) void mi0a00090(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a00090);	};	//93
__attribute__((weak)) void mi0a00101(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a00101);	};	//94
__attribute__((weak)) void mi0a00180(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a00180);	};	//95
__attribute__((weak)) void mi0a00181(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a00181);	};	//96
__attribute__((weak)) void mi0a00220(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a00220);	};	//97
__attribute__((weak)) void mi0a00808(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a00808);	};	//98
__attribute__((weak)) void mi0a00820(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a00820);	};	//99
__attribute__((weak)) void mi0a01001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a01001);	};	//100
__attribute__((weak)) void mi0a01008(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a01008);	};	//101
__attribute__((weak)) void mi0a01020(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a01020);	};	//102
__attribute__((weak)) void mi0a01081(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a01081);	};	//103
__attribute__((weak)) void mi0a010a0(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a010a0);	};	//104
__attribute__((weak)) void mi0a01181(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a01181);	};	//105
__attribute__((weak)) void mi0a03081(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a03081);	};	//106
__attribute__((weak)) void mi0a03120(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a03120);	};	//107
__attribute__((weak)) void mi0a03180(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a03180);	};	//108
__attribute__((weak)) void mi0a03c20(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a03c20);	};	//109
__attribute__((weak)) void mi0a04020(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a04020);	};	//110
__attribute__((weak)) void mi0a10181(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a10181);	};	//111
__attribute__((weak)) void mi0a10801(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a10801);	};	//112
__attribute__((weak)) void mi0a11801(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x00a11801);	};	//113
__attribute__((weak)) void mi1000000(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01000000);	};	//114
__attribute__((weak)) void mi1000001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01000001);	};	//115
__attribute__((weak)) void mi1000180(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01000180);	};	//116
__attribute__((weak)) void mi1000208(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01000208);	};	//117
__attribute__((weak)) void mi1001280(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01001280);	};	//118
__attribute__((weak)) void mi1002200(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01002200);	};	//119
__attribute__((weak)) void mi1008001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01008001);	};	//120
__attribute__((weak)) void mi1200209(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01200209);	};	//121
__attribute__((weak)) void mi1200801(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01200801);	};	//122
__attribute__((weak)) void mi1203200(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01203200);	};	//123
__attribute__((weak)) void mi1400020(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01400020);	};	//124
__attribute__((weak)) void mi1418001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01418001);	};	//125
__attribute__((weak)) void mi1440090(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01440090);	};	//126
__attribute__((weak)) void mi1801200(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01801200);	};	//127
__attribute__((weak)) void mi1a00220(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x01a00220);	};	//128
__attribute__((weak)) void mi3200209(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x03200209);	};	//129
__attribute__((weak)) void mi4000001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x04000001);	};	//130
__attribute__((weak)) void mi4000020(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x04000020);	};	//131
__attribute__((weak)) void mi4000100(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x04000100);	};	//132
__attribute__((weak)) void mi4018001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x04018001);	};	//133
__attribute__((weak)) void mi4018801(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x04018801);	};	//134
__attribute__((weak)) void mi4800001(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x04800001);	};	//135
__attribute__((weak)) void mi4800801(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x04800801);	};	//136
__attribute__((weak)) void mi4810002(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x04810002);	};	//137
__attribute__((weak)) void mi8010004(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x08010004);	};	//138
__attribute__((weak)) void mi8241004(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x08241004);	};	//139
__attribute__((weak)) void mi8800004(IK1302& dev, mtick_t _I, int mi) {	return mi_exec(dev, _I, 0x08800004);	};	//140

static const jtbl_item_t jt_2[17*4] = {
 mi0000000, mi0800001, mi0a00820, mi0040020,
 mi0a03120, mi0203081, mi0a00181, mi0803800,
 mi0818001, mi0800400, mi0a00089, mi0a03c20,
 mi0800820, mi0080020, mi0800120, mi1400020,
 mi0800081, mi0210801, mi0040000, mi0058001,
 mi0808001, mi0a03081, mi0a01081, mi0a01181,
 mi0040090, mi0800401, mi0a00081, mi0040001,
 mi0800801, mi1000000, mi0800100, mi1200801,
 mi0013c01, mi0800008, mi0a00088, mi0010200,
 mi0800040, mi0800280, mi1801200, mi1000208,
 mi0080001, mi0a00082, mi0a01008, mi1000001,
 mi0a00808, mi0900001, mi8010004, mi0080820,
 mi0800002, mi0140002, mi0008000, mi0a00090,
 mi0a00220, mi0801001, mi1203200, mi4800001,
 mi0011801, mi1008001, mi0a04020, mi4800801,
 mi0840801, mi0840020, mi0013081, mi0010801,
 mi0818180, mi0800180, mi0a00081, mi0800001};

void mi2_exec(IK1302& dev, mtick_t _I, int idx) { mi145_exec(dev, _I, idx, jt_2); };
static const jtbl_item_t jt_3[17*4] = {
 mi0000000, mi0800001, mi0040020, mi1440090,
 mi0a00081, mi1000000, mi1400020, mi0800008,
 mi0a03180, mi1002200, mi0800400, mi1418001,
 mi0080020, mi0841020, mi0203100, mi0203088,
 mi0a00820, mi0800120, mi08001c0, mi0810081,
 mi0a00089, mi0800401, mi0a010a0, mi0a01081,
 mi0818001, mi1a00220, mi0201100, mi0203420,
 mi0008000, mi0801020, mi0201420, mi0801190,
 mi0040000, mi0080820, mi0800002, mi0140002,
 mi0800100, mi0a03c20, mi0a00808, mi0a01008,
 mi0200540, mi0601209, mi0083100, mi0a03081,
 mi8800004, mi0058001, mi1001280, mi1008001,
 mi1200209, mi4018001, mi0040002, mi1000001,
 mi0010200, mi0800840, mi0a01181, mi4018801,
 mi0a10181, mi0800801, mi0040001, mi0011190,
 mi0858001, mi0040020, mi3200209, mi08000c0,
 mi4000020, mi0600081, mi1000000, mi1000180};

void mi3_exec(IK1302& dev, mtick_t _I, int idx) { mi145_exec(dev, _I, idx, jt_3); };
static const jtbl_item_t jt_6[17*4] = {
 mi0000000, mi0800008, mi0040020, mi0800001,
 mi0800021, mi0080020, mi0a00028, mi0040100,
 mi4000100, mi0010100, mi0a00101, mi0201089,
 mi0213201, mi0800004, mi0800800, mi0800820,
 mi0200088, mi4810002, mi0a00820, mi0800400,
 mi0801000, mi0100000, mi8800004, mi0008000,
 mi1400020, mi0800005, mi4000020, mi0a00180,
 mi0100000, mi4000001, mi8241004, mi0400000,
 mi0080001, mi0040001, mi0212801, mi0200808,
 mi0800000, mi0010020, mi0a00808, mi0040090,
 mi0a01008, mi0800401, mi0a00081, mi0a01081,
 mi0803400, mi0a01001, mi0a11801, mi0011001,
 mi0a10801, mi0213801, mi0098001, mi0818001,
 mi0800420, mi0880090, mi0203c08, mi0200809,
 mi0a00089, mi0203090, mi0840090, mi0810002,
 mi0210801, mi0210081, mi0010000, mi0200090,
 mi0210081, mi0212801, mi0a01020, mi0a01020};

void mi6_exec(IK1302& dev, mtick_t _I, int idx) { mi145_exec(dev, _I, idx, jt_6); };

}

#if defined (MI145_TB)
int main(void) {
	return 0;
}
#endif
