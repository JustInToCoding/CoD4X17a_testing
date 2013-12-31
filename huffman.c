/*
===========================================================================
    Copyright (C) 1999-2005 Id Software, Inc.

    This file is part of CoD4X17a-Server source code.

    CoD4X17a-Server source code is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    CoD4X17a-Server source code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
===========================================================================
*/



#include <string.h>

#include "huffman.h"

/* This is based on the Adaptive Huffman algorithm described in Sayood's Data
 * Compression book.  The ranks are not actually stored, but implicitly defined
 * by the location of a node within a doubly-linked list */

static int bloc = 0;

/* Add a bit to the output file (buffered) */
static void add_bit( char bit, byte *fout ) {
	if ( ( bloc & 7 ) == 0 ) {
		fout[( bloc >> 3 )] = 0;
	}
	fout[( bloc >> 3 )] |= bit << ( bloc & 7 );
	bloc++;
}

/* Receive one bit from the input file (buffered) */
static int get_bit( byte *fin ) {
	int t;
	t = ( fin[( bloc >> 3 )] >> ( bloc & 7 ) ) & 0x1;
	bloc++;
	return t;
}

/* Get a symbol */

static void Huff_offsetReceive( node_t *node, int *ch, byte *fin, int *offset ) {
	bloc = *offset;
	while ( node && node->symbol == INTERNAL_NODE ) {

		if ( get_bit( fin ) ) {
			node = node->right;

		} else {
			node = node->left;

		}
	}
	if ( !node ) {
		*ch = 0;
//		Com_PrintError("Illegal tree!\n");
		return;

	}
	*ch = node->symbol;
	*offset = bloc;
}



/* Send the prefix code for this node */
static void Huff_send( node_t *node, node_t *child, byte *fout ) {
	if ( node->parent ) {
		Huff_send( node->parent, node, fout );
	}
	if ( child ) {
		if ( node->right == child ) {
			add_bit( 1, fout );
		} else {
			add_bit( 0, fout );
		}
	}
}

static void Huff_offsetTransmit( huff_t *huff, int ch, byte *fout, int *offset ) {
	bloc = *offset;
	Huff_send( huff->loc[ch], NULL, fout );
	*offset = bloc;
}


static void Huff_Init( huff_t *huff ) {

	Com_Memset( huff, 0, sizeof( huff_t ));

	// Initialize the tree & list with the NYT node
	huff->tree = &( huff->nodeList[huff->blocNode++] );
	huff->loc[NYT] = huff->tree;

	huff->tree->symbol = NYT;

	huff->tree->weight = 0;
	huff->tree->parent = NULL;
	huff->tree->left = NULL;
	huff->tree->right = NULL;
}



int msg_hData[256] = {
	274054,			//0
	68777,			//1
	40460,			//2
	40266,			//3
	48059,			//4
	39006,			//5
	48630,			//6
	27692,			//7
	17712,			//8
	15439,			//9
	12386,			//10
	10758,			//11
	9420,			//12
	9979,			//13
	9346,			//14
	15256,			//15
	13184,			//16
	14319,			//17
	7750,			//18
	7221,			//19
	6095,			//20
	5666,			//21
	12606,			//22
	7263,			//23
	7322,			//24
	5807,			//25
	11628,			//26
	6199,			//27
	7826,			//28
	6349,			//29
	7698,			//30
	9656,			//31
	28968,			//32
	5164,			//33
	13629,			//34
	6058,			//35
	4745,			//36
	4519,			//37
	5199,			//38
	4807,			//39
	5323,			//40
	3433,			//41
	3455,			//42
	3563,			//43
	6979,			//44
	5229,			//45
	5002,			//46
	4423,			//47
	14108,			//48
	13631,			//49
	11908,			//50
	11801,			//51
	10261,			//52
	7635,			//53
	7215,			//54
	7218,			//55
	9353,			//56
	6161,			//57
	5689,			//58
	4649,			//59
	5026,			//60
	5866,			//61
	8002,			//62
	10534,			//63
	15381,			//64
	8874,			//65
	11798,			//66
	7199,			//67
	12814,			//68
	6103,			//69
	4982,			//70
	5972,			//71
	6779,			//72
	4929,			//73
	5333,			//74
	3503,			//75
	4345,			//76
	6098,			//77
	14117,			//78
	16440,			//79
	6446,			//80
	3062,			//81
	4695,			//82
	3085,			//83
	4198,			//84
	4013,			//85
	3878,			//86
	3414,			//87
	5514,			//88
	4092,			//89
	3261,			//90
	4740,			//91
	4544,			//92
	3127,			//93
	3385,			//94
	7688,			//95
	11126,			//96
	6417,			//97
	5297,			//98
	4529,			//99
	6333,			//100
	4210,			//101
	7056,			//102
	4658,			//103
	6190,			//104
	3512,			//105
	2843,			//106
	3479,			//107
	9369,			//108
	5203,			//109
	4980,			//110
	5881,			//111
	7509,			//112
	4292,			//113
	6097,			//114
	5492,			//115
	4648,			//116
	2996,			//117
	4988,			//118
	4163,			//119
	6534,			//120
	4001,			//121
	4342,			//122
	4488,			//123
	6039,			//124
	4827,			//125
	7112,			//126
	8654,			//127
	26712,			//128
	8688,			//129
	9677,			//130
	9368,			//131
	7209,			//132
	3399,			//133
	4473,			//134
	4677,			//135
	11087,			//136
	4094,			//137
	3404,			//138
	4176,			//139
	6733,			//140
	3702,			//141
	11420,			//142
	4867,			//143
	5968,			//144
	3475,			//145
	3722,			//146
	3560,			//147
	4571,			//148
	2720,			//149
	3189,			//150
	3099,			//151
	4595,			//152
	4044,			//153
	4402,			//154
	3889,			//155
	4989,			//156
	3186,			//157
	3153,			//158
	5387,			//159
	8020,			//160
	3322,			//161
	3775,			//162
	2886,			//163
	4191,			//164
	2879,			//165
	3110,			//166
	2576,			//167
	3693,			//168
	2436,			//169
	4935,			//170
	3017,			//171
	3538,			//172
	5688,			//173
	3444,			//174
	3410,			//175
	9170,			//176
	4708,			//177
	3425,			//178
	3273,			//179
	3684,			//180
	4564,			//181
	6957,			//182
	4817,			//183
	5224,			//184
	3285,			//185
	3143,			//186
	4227,			//187
	5630,			//188
	6053,			//189
	5851,			//190
	6507,			//191
	13692,			//192
	8270,			//193
	8260,			//194
	5583,			//195
	7568,			//196
	4082,			//197
	3984,			//198
	4574,			//199
	6440,			//200
	3533,			//201
	2992,			//202
	2708,			//203
	5190,			//204
	3889,			//205
	3799,			//206
	4582,			//207
	6020,			//208
	3464,			//209
	4431,			//210
	3495,			//211
	2906,			//212
	2243,			//213
	3856,			//214
	3321,			//215
	8759,			//216
	3928,			//217
	2905,			//218
	3875,			//219
	4382,			//220
	3885,			//221
	5869,			//222
	6235,			//223
	10685,			//224
	4433,			//225
	4639,			//226
	4305,			//227
	4683,			//228
	2849,			//229
	3379,			//230
	4683,			//231
	5477,			//232
	4127,			//233
	3853,			//234
	3515,			//235
	4913,			//236
	3601,			//237
	5237,			//238
	6617,			//239
	9019,			//240
	4857,			//241
	4112,			//242
	5180,			//243
	5998,			//244
	4925,			//245
	4986,			//246
	6365,			//247
	7930,			//248
	5948,			//249
	8085,			//250
	7732,			//251
	8643,			//252
	8901,			//253
	9653,			//254
	32647,			//255

};

static huff_t		msgHuff;

int MSG_ReadBitsCompress(const byte* input, byte* outputBuf, int readsize){

    readsize = readsize * 8;
    byte *outptr = outputBuf;

    int get;
    int offset;
    int i;

    if(readsize <= 0){
        return 0;
    }

    for(offset = 0, i = 0; readsize > offset; i++){
        Huff_offsetReceive( msgHuff.tree, &get, (byte*)input, &offset);
        *outptr = (byte)get;
        outptr++;
    }
    return i;
}

int MSG_WriteBitsCompress( char dummy, const byte *datasrc, byte *buffdest, int bytecount){

    int offset;
    int i;

    if(bytecount <= 0){
        return 0;
    }

    for(offset = 0, i = 0; i < bytecount; i++){
        Huff_offsetTransmit( &msgHuff, (int)datasrc[i], buffdest, &offset );
    }
    return (offset + 7) / 8;
}


static void Huff_BuildFromData(huff_t* huff, const int* msg_hData)
{
  huff->blocNode = 513;
  huff->nodeList[511].parent = &huff->nodeList[512];
  huff->tree = &huff->nodeList[512];
  huff->nodeList[508].left = &huff->nodeList[1];
  huff->loc[0] = &huff->nodeList[1];
  huff->nodeList[493].left = &huff->nodeList[2];
  huff->loc[1] = &huff->nodeList[2];
  huff->nodeList[480].left = &huff->nodeList[3];
  huff->loc[2] = &huff->nodeList[3];
  huff->nodeList[479].right = &huff->nodeList[4];
  huff->loc[3] = &huff->nodeList[4];
  huff->nodeList[484].left = &huff->nodeList[5];
  huff->loc[4] = &huff->nodeList[5];
  huff->nodeList[478].left = &huff->nodeList[6];
  huff->loc[5] = &huff->nodeList[6];
  huff->nodeList[485].left = &huff->nodeList[7];
  huff->loc[6] = &huff->nodeList[7];
  huff->nodeList[465].left = &huff->nodeList[8];
  huff->loc[7] = &huff->nodeList[8];
  huff->nodeList[442].right = &huff->nodeList[9];
  huff->loc[8] = &huff->nodeList[9];
  huff->nodeList[434].right = &huff->nodeList[10];
  huff->loc[9] = &huff->nodeList[10];
  huff->nodeList[416].left = &huff->nodeList[11];
  huff->loc[10] = &huff->nodeList[11];
  huff->nodeList[403].right = &huff->nodeList[12];
  huff->loc[11] = &huff->nodeList[12];
  huff->nodeList[392].left = &huff->nodeList[13];
  huff->loc[12] = &huff->nodeList[13];
  huff->nodeList[398].left = &huff->nodeList[14];
  huff->loc[13] = &huff->nodeList[14];
  huff->nodeList[388].right = &huff->nodeList[15];
  huff->loc[14] = &huff->nodeList[15];
  huff->nodeList[432].right = &huff->nodeList[16];
  huff->loc[15] = &huff->nodeList[16];
  huff->nodeList[421].right = &huff->nodeList[17];
  huff->loc[16] = &huff->nodeList[17];
  huff->nodeList[429].left = &huff->nodeList[18];
  huff->loc[17] = &huff->nodeList[18];
  huff->nodeList[369].right = &huff->nodeList[19];
  huff->loc[18] = &huff->nodeList[19];
  huff->nodeList[363].left = &huff->nodeList[20];
  huff->loc[19] = &huff->nodeList[20];
  huff->nodeList[340].right = &huff->nodeList[21];
  huff->loc[20] = &huff->nodeList[21];
  huff->nodeList[330].right = &huff->nodeList[22];
  huff->loc[21] = &huff->nodeList[22];
  huff->nodeList[417].right = &huff->nodeList[23];
  huff->loc[22] = &huff->nodeList[23];
  huff->nodeList[363].right = &huff->nodeList[24];
  huff->loc[23] = &huff->nodeList[24];
  huff->nodeList[364].left = &huff->nodeList[25];
  huff->loc[24] = &huff->nodeList[25];
  huff->nodeList[333].left = &huff->nodeList[26];
  huff->loc[25] = &huff->nodeList[26];
  huff->nodeList[408].right = &huff->nodeList[27];
  huff->loc[26] = &huff->nodeList[27];
  huff->nodeList[344].left = &huff->nodeList[28];
  huff->loc[27] = &huff->nodeList[28];
  huff->nodeList[371].right = &huff->nodeList[29];
  huff->loc[28] = &huff->nodeList[29];
  huff->nodeList[347].left = &huff->nodeList[30];
  huff->loc[29] = &huff->nodeList[30];
  huff->nodeList[368].left = &huff->nodeList[31];
  huff->loc[30] = &huff->nodeList[31];
  huff->nodeList[394].left = &huff->nodeList[32];
  huff->loc[31] = &huff->nodeList[32];
  huff->nodeList[468].left = &huff->nodeList[33];
  huff->loc[32] = &huff->nodeList[33];
  huff->nodeList[321].left = &huff->nodeList[34];
  huff->loc[33] = &huff->nodeList[34];
  huff->nodeList[423].right = &huff->nodeList[35];
  huff->loc[34] = &huff->nodeList[35];
  huff->nodeList[340].left = &huff->nodeList[36];
  huff->loc[35] = &huff->nodeList[36];
  huff->nodeList[312].right = &huff->nodeList[37];
  huff->loc[36] = &huff->nodeList[37];
  huff->nodeList[303].left = &huff->nodeList[38];
  huff->loc[37] = &huff->nodeList[38];
  huff->nodeList[322].right = &huff->nodeList[39];
  huff->loc[38] = &huff->nodeList[39];
  huff->nodeList[313].left = &huff->nodeList[40];
  huff->loc[39] = &huff->nodeList[40];
  huff->nodeList[326].left = &huff->nodeList[41];
  huff->loc[40] = &huff->nodeList[41];
  huff->nodeList[275].right = &huff->nodeList[42];
  huff->loc[41] = &huff->nodeList[42];
  huff->nodeList[276].right = &huff->nodeList[43];
  huff->loc[42] = &huff->nodeList[43];
  huff->nodeList[282].left = &huff->nodeList[44];
  huff->loc[43] = &huff->nodeList[44];
  huff->nodeList[357].right = &huff->nodeList[45];
  huff->loc[44] = &huff->nodeList[45];
  huff->nodeList[324].left = &huff->nodeList[46];
  huff->loc[45] = &huff->nodeList[46];
  huff->nodeList[320].left = &huff->nodeList[47];
  huff->loc[46] = &huff->nodeList[47];
  huff->nodeList[300].right = &huff->nodeList[48];
  huff->loc[47] = &huff->nodeList[48];
  huff->nodeList[427].left = &huff->nodeList[49];
  huff->loc[48] = &huff->nodeList[49];
  huff->nodeList[424].left = &huff->nodeList[50];
  huff->loc[49] = &huff->nodeList[50];
  huff->nodeList[411].right = &huff->nodeList[51];
  huff->loc[50] = &huff->nodeList[51];
  huff->nodeList[411].left = &huff->nodeList[52];
  huff->loc[51] = &huff->nodeList[52];
  huff->nodeList[399].left = &huff->nodeList[53];
  huff->loc[52] = &huff->nodeList[53];
  huff->nodeList[367].left = &huff->nodeList[54];
  huff->loc[53] = &huff->nodeList[54];
  huff->nodeList[362].left = &huff->nodeList[55];
  huff->loc[54] = &huff->nodeList[55];
  huff->nodeList[362].right = &huff->nodeList[56];
  huff->loc[55] = &huff->nodeList[56];
  huff->nodeList[389].left = &huff->nodeList[57];
  huff->loc[56] = &huff->nodeList[57];
  huff->nodeList[343].left = &huff->nodeList[58];
  huff->loc[57] = &huff->nodeList[58];
  huff->nodeList[331].right = &huff->nodeList[59];
  huff->loc[58] = &huff->nodeList[59];
  huff->nodeList[308].left = &huff->nodeList[60];
  huff->loc[59] = &huff->nodeList[60];
  huff->nodeList[320].right = &huff->nodeList[61];
  huff->loc[60] = &huff->nodeList[61];
  huff->nodeList[334].left = &huff->nodeList[62];
  huff->loc[61] = &huff->nodeList[62];
  huff->nodeList[373].left = &huff->nodeList[63];
  huff->loc[62] = &huff->nodeList[63];
  huff->nodeList[401].right = &huff->nodeList[64];
  huff->loc[63] = &huff->nodeList[64];
  huff->nodeList[433].right = &huff->nodeList[65];
  huff->loc[64] = &huff->nodeList[65];
  huff->nodeList[383].left = &huff->nodeList[66];
  huff->loc[65] = &huff->nodeList[66];
  huff->nodeList[410].right = &huff->nodeList[67];
  huff->loc[66] = &huff->nodeList[67];
  huff->nodeList[361].left = &huff->nodeList[68];
  huff->loc[67] = &huff->nodeList[68];
  huff->nodeList[419].left = &huff->nodeList[69];
  huff->loc[68] = &huff->nodeList[69];
  huff->nodeList[342].left = &huff->nodeList[70];
  huff->loc[69] = &huff->nodeList[70];
  huff->nodeList[318].left = &huff->nodeList[71];
  huff->loc[70] = &huff->nodeList[71];
  huff->nodeList[337].left = &huff->nodeList[72];
  huff->loc[71] = &huff->nodeList[72];
  huff->nodeList[353].right = &huff->nodeList[73];
  huff->loc[72] = &huff->nodeList[73];
  huff->nodeList[316].right = &huff->nodeList[74];
  huff->loc[73] = &huff->nodeList[74];
  huff->nodeList[326].right = &huff->nodeList[75];
  huff->loc[74] = &huff->nodeList[75];
  huff->nodeList[279].left = &huff->nodeList[76];
  huff->loc[75] = &huff->nodeList[76];
  huff->nodeList[299].left = &huff->nodeList[77];
  huff->loc[76] = &huff->nodeList[77];
  huff->nodeList[341].right = &huff->nodeList[78];
  huff->loc[77] = &huff->nodeList[78];
  huff->nodeList[427].right = &huff->nodeList[79];
  huff->loc[78] = &huff->nodeList[79];
  huff->nodeList[438].right = &huff->nodeList[80];
  huff->loc[79] = &huff->nodeList[80];
  huff->nodeList[349].left = &huff->nodeList[81];
  huff->loc[80] = &huff->nodeList[81];
  huff->nodeList[265].left = &huff->nodeList[82];
  huff->loc[81] = &huff->nodeList[82];
  huff->nodeList[311].left = &huff->nodeList[83];
  huff->loc[82] = &huff->nodeList[83];
  huff->nodeList[265].right = &huff->nodeList[84];
  huff->loc[83] = &huff->nodeList[84];
  huff->nodeList[296].left = &huff->nodeList[85];
  huff->loc[84] = &huff->nodeList[85];
  huff->nodeList[291].left = &huff->nodeList[86];
  huff->loc[85] = &huff->nodeList[86];
  huff->nodeList[287].right = &huff->nodeList[87];
  huff->loc[86] = &huff->nodeList[87];
  huff->nodeList[274].right = &huff->nodeList[88];
  huff->loc[87] = &huff->nodeList[88];
  huff->nodeList[328].right = &huff->nodeList[89];
  huff->loc[88] = &huff->nodeList[89];
  huff->nodeList[292].right = &huff->nodeList[90];
  huff->loc[89] = &huff->nodeList[90];
  huff->nodeList[269].right = &huff->nodeList[91];
  huff->loc[90] = &huff->nodeList[91];
  huff->nodeList[312].left = &huff->nodeList[92];
  huff->loc[91] = &huff->nodeList[92];
  huff->nodeList[304].left = &huff->nodeList[93];
  huff->loc[92] = &huff->nodeList[93];
  huff->nodeList[267].left = &huff->nodeList[94];
  huff->loc[93] = &huff->nodeList[94];
  huff->nodeList[272].right = &huff->nodeList[95];
  huff->loc[94] = &huff->nodeList[95];
  huff->nodeList[367].right = &huff->nodeList[96];
  huff->loc[95] = &huff->nodeList[96];
  huff->nodeList[405].right = &huff->nodeList[97];
  huff->loc[96] = &huff->nodeList[97];
  huff->nodeList[348].left = &huff->nodeList[98];
  huff->loc[97] = &huff->nodeList[98];
  huff->nodeList[325].right = &huff->nodeList[99];
  huff->loc[98] = &huff->nodeList[99];
  huff->nodeList[303].right = &huff->nodeList[100];
  huff->loc[99] = &huff->nodeList[100];
  huff->nodeList[346].left = &huff->nodeList[101];
  huff->loc[100] = &huff->nodeList[101];
  huff->nodeList[296].right = &huff->nodeList[102];
  huff->loc[101] = &huff->nodeList[102];
  huff->nodeList[359].left = &huff->nodeList[103];
  huff->loc[102] = &huff->nodeList[103];
  huff->nodeList[308].right = &huff->nodeList[104];
  huff->loc[103] = &huff->nodeList[104];
  huff->nodeList[343].right = &huff->nodeList[105];
  huff->loc[104] = &huff->nodeList[105];
  huff->nodeList[279].right = &huff->nodeList[106];
  huff->loc[105] = &huff->nodeList[106];
  huff->nodeList[260].right = &huff->nodeList[107];
  huff->loc[106] = &huff->nodeList[107];
  huff->nodeList[278].left = &huff->nodeList[108];
  huff->loc[107] = &huff->nodeList[108];
  huff->nodeList[391].left = &huff->nodeList[109];
  huff->loc[108] = &huff->nodeList[109];
  huff->nodeList[323].left = &huff->nodeList[110];
  huff->loc[109] = &huff->nodeList[110];
  huff->nodeList[317].right = &huff->nodeList[111];
  huff->loc[110] = &huff->nodeList[111];
  huff->nodeList[335].left = &huff->nodeList[112];
  huff->loc[111] = &huff->nodeList[112];
  huff->nodeList[365].right = &huff->nodeList[113];
  huff->loc[112] = &huff->nodeList[113];
  huff->nodeList[297].right = &huff->nodeList[114];
  huff->loc[113] = &huff->nodeList[114];
  huff->nodeList[341].left = &huff->nodeList[115];
  huff->loc[114] = &huff->nodeList[115];
  huff->nodeList[328].left = &huff->nodeList[116];
  huff->loc[115] = &huff->nodeList[116];
  huff->nodeList[307].right = &huff->nodeList[117];
  huff->loc[116] = &huff->nodeList[117];
  huff->nodeList[264].left = &huff->nodeList[118];
  huff->loc[117] = &huff->nodeList[118];
  huff->nodeList[319].left = &huff->nodeList[119];
  huff->loc[118] = &huff->nodeList[119];
  huff->nodeList[294].right = &huff->nodeList[120];
  huff->loc[119] = &huff->nodeList[120];
  huff->nodeList[350].right = &huff->nodeList[121];
  huff->loc[120] = &huff->nodeList[121];
  huff->nodeList[290].right = &huff->nodeList[122];
  huff->loc[121] = &huff->nodeList[122];
  huff->nodeList[298].right = &huff->nodeList[123];
  huff->loc[122] = &huff->nodeList[123];
  huff->nodeList[302].right = &huff->nodeList[124];
  huff->loc[123] = &huff->nodeList[124];
  huff->nodeList[339].left = &huff->nodeList[125];
  huff->loc[124] = &huff->nodeList[125];
  huff->nodeList[314].left = &huff->nodeList[126];
  huff->loc[125] = &huff->nodeList[126];
  huff->nodeList[360].left = &huff->nodeList[127];
  huff->loc[126] = &huff->nodeList[127];
  huff->nodeList[380].left = &huff->nodeList[128];
  huff->loc[127] = &huff->nodeList[128];
  huff->nodeList[462].right = &huff->nodeList[129];
  huff->loc[128] = &huff->nodeList[129];
  huff->nodeList[380].right = &huff->nodeList[130];
  huff->loc[129] = &huff->nodeList[130];
  huff->nodeList[394].right = &huff->nodeList[131];
  huff->loc[130] = &huff->nodeList[131];
  huff->nodeList[390].right = &huff->nodeList[132];
  huff->loc[131] = &huff->nodeList[132];
  huff->nodeList[361].right = &huff->nodeList[133];
  huff->loc[132] = &huff->nodeList[133];
  huff->nodeList[273].left = &huff->nodeList[134];
  huff->loc[133] = &huff->nodeList[134];
  huff->nodeList[302].left = &huff->nodeList[135];
  huff->loc[134] = &huff->nodeList[135];
  huff->nodeList[309].left = &huff->nodeList[136];
  huff->loc[135] = &huff->nodeList[136];
  huff->nodeList[405].left = &huff->nodeList[137];
  huff->loc[136] = &huff->nodeList[137];
  huff->nodeList[293].left = &huff->nodeList[138];
  huff->loc[137] = &huff->nodeList[138];
  huff->nodeList[273].right = &huff->nodeList[139];
  huff->loc[138] = &huff->nodeList[139];
  huff->nodeList[295].left = &huff->nodeList[140];
  huff->loc[139] = &huff->nodeList[140];
  huff->nodeList[352].right = &huff->nodeList[141];
  huff->loc[140] = &huff->nodeList[141];
  huff->nodeList[284].left = &huff->nodeList[142];
  huff->loc[141] = &huff->nodeList[142];
  huff->nodeList[407].right = &huff->nodeList[143];
  huff->loc[142] = &huff->nodeList[143];
  huff->nodeList[315].left = &huff->nodeList[144];
  huff->loc[143] = &huff->nodeList[144];
  huff->nodeList[336].right = &huff->nodeList[145];
  huff->loc[144] = &huff->nodeList[145];
  huff->nodeList[277].right = &huff->nodeList[146];
  huff->loc[145] = &huff->nodeList[146];
  huff->nodeList[284].right = &huff->nodeList[147];
  huff->loc[146] = &huff->nodeList[147];
  huff->nodeList[281].right = &huff->nodeList[148];
  huff->loc[147] = &huff->nodeList[148];
  huff->nodeList[305].left = &huff->nodeList[149];
  huff->loc[148] = &huff->nodeList[149];
  huff->nodeList[260].left = &huff->nodeList[150];
  huff->loc[149] = &huff->nodeList[150];
  huff->nodeList[269].left = &huff->nodeList[151];
  huff->loc[150] = &huff->nodeList[151];
  huff->nodeList[266].left = &huff->nodeList[152];
  huff->loc[151] = &huff->nodeList[152];
  huff->nodeList[306].right = &huff->nodeList[153];
  huff->loc[152] = &huff->nodeList[153];
  huff->nodeList[291].right = &huff->nodeList[154];
  huff->loc[153] = &huff->nodeList[154];
  huff->nodeList[300].left = &huff->nodeList[155];
  huff->loc[154] = &huff->nodeList[155];
  huff->nodeList[289].left = &huff->nodeList[156];
  huff->loc[155] = &huff->nodeList[156];
  huff->nodeList[319].right = &huff->nodeList[157];
  huff->loc[156] = &huff->nodeList[157];
  huff->nodeList[268].right = &huff->nodeList[158];
  huff->loc[157] = &huff->nodeList[158];
  huff->nodeList[268].left = &huff->nodeList[159];
  huff->loc[158] = &huff->nodeList[159];
  huff->nodeList[327].left = &huff->nodeList[160];
  huff->loc[159] = &huff->nodeList[160];
  huff->nodeList[373].right = &huff->nodeList[161];
  huff->loc[160] = &huff->nodeList[161];
  huff->nodeList[271].right = &huff->nodeList[162];
  huff->loc[161] = &huff->nodeList[162];
  huff->nodeList[285].left = &huff->nodeList[163];
  huff->loc[162] = &huff->nodeList[163];
  huff->nodeList[262].left = &huff->nodeList[164];
  huff->loc[163] = &huff->nodeList[164];
  huff->nodeList[295].right = &huff->nodeList[165];
  huff->loc[164] = &huff->nodeList[165];
  huff->nodeList[261].right = &huff->nodeList[166];
  huff->loc[165] = &huff->nodeList[166];
  huff->nodeList[266].right = &huff->nodeList[167];
  huff->loc[166] = &huff->nodeList[167];
  huff->nodeList[259].left = &huff->nodeList[168];
  huff->loc[167] = &huff->nodeList[168];
  huff->nodeList[283].right = &huff->nodeList[169];
  huff->loc[168] = &huff->nodeList[169];
  huff->nodeList[258].right = &huff->nodeList[170];
  huff->loc[169] = &huff->nodeList[170];
  huff->nodeList[317].left = &huff->nodeList[171];
  huff->loc[170] = &huff->nodeList[171];
  huff->nodeList[264].right = &huff->nodeList[172];
  huff->loc[171] = &huff->nodeList[172];
  huff->nodeList[281].left = &huff->nodeList[173];
  huff->loc[172] = &huff->nodeList[173];
  huff->nodeList[331].left = &huff->nodeList[174];
  huff->loc[173] = &huff->nodeList[174];
  huff->nodeList[276].left = &huff->nodeList[175];
  huff->loc[174] = &huff->nodeList[175];
  huff->nodeList[274].left = &huff->nodeList[176];
  huff->loc[175] = &huff->nodeList[176];
  huff->nodeList[386].right = &huff->nodeList[177];
  huff->loc[176] = &huff->nodeList[177];
  huff->nodeList[311].right = &huff->nodeList[178];
  huff->loc[177] = &huff->nodeList[178];
  huff->nodeList[275].left = &huff->nodeList[179];
  huff->loc[178] = &huff->nodeList[179];
  huff->nodeList[270].left = &huff->nodeList[180];
  huff->loc[179] = &huff->nodeList[180];
  huff->nodeList[283].left = &huff->nodeList[181];
  huff->loc[180] = &huff->nodeList[181];
  huff->nodeList[304].right = &huff->nodeList[182];
  huff->loc[181] = &huff->nodeList[182];
  huff->nodeList[356].right = &huff->nodeList[183];
  huff->loc[182] = &huff->nodeList[183];
  huff->nodeList[313].right = &huff->nodeList[184];
  huff->loc[183] = &huff->nodeList[184];
  huff->nodeList[323].right = &huff->nodeList[185];
  huff->loc[184] = &huff->nodeList[185];
  huff->nodeList[270].right = &huff->nodeList[186];
  huff->loc[185] = &huff->nodeList[186];
  huff->nodeList[267].right = &huff->nodeList[187];
  huff->loc[186] = &huff->nodeList[187];
  huff->nodeList[297].left = &huff->nodeList[188];
  huff->loc[187] = &huff->nodeList[188];
  huff->nodeList[330].left = &huff->nodeList[189];
  huff->loc[188] = &huff->nodeList[189];
  huff->nodeList[339].right = &huff->nodeList[190];
  huff->loc[189] = &huff->nodeList[190];
  huff->nodeList[333].right = &huff->nodeList[191];
  huff->loc[190] = &huff->nodeList[191];
  huff->nodeList[350].left = &huff->nodeList[192];
  huff->loc[191] = &huff->nodeList[192];
  huff->nodeList[424].right = &huff->nodeList[193];
  huff->loc[192] = &huff->nodeList[193];
  huff->nodeList[376].right = &huff->nodeList[194];
  huff->loc[193] = &huff->nodeList[194];
  huff->nodeList[376].left = &huff->nodeList[195];
  huff->loc[194] = &huff->nodeList[195];
  huff->nodeList[329].right = &huff->nodeList[196];
  huff->loc[195] = &huff->nodeList[196];
  huff->nodeList[366].left = &huff->nodeList[197];
  huff->loc[196] = &huff->nodeList[197];
  huff->nodeList[292].left = &huff->nodeList[198];
  huff->loc[197] = &huff->nodeList[198];
  huff->nodeList[290].left = &huff->nodeList[199];
  huff->loc[198] = &huff->nodeList[199];
  huff->nodeList[305].right = &huff->nodeList[200];
  huff->loc[199] = &huff->nodeList[200];
  huff->nodeList[348].right = &huff->nodeList[201];
  huff->loc[200] = &huff->nodeList[201];
  huff->nodeList[280].right = &huff->nodeList[202];
  huff->loc[201] = &huff->nodeList[202];
  huff->nodeList[263].right = &huff->nodeList[203];
  huff->loc[202] = &huff->nodeList[203];
  huff->nodeList[259].right = &huff->nodeList[204];
  huff->loc[203] = &huff->nodeList[204];
  huff->nodeList[322].left = &huff->nodeList[205];
  huff->loc[204] = &huff->nodeList[205];
  huff->nodeList[288].right = &huff->nodeList[206];
  huff->loc[205] = &huff->nodeList[206];
  huff->nodeList[285].right = &huff->nodeList[207];
  huff->loc[206] = &huff->nodeList[207];
  huff->nodeList[306].left = &huff->nodeList[208];
  huff->loc[207] = &huff->nodeList[208];
  huff->nodeList[338].right = &huff->nodeList[209];
  huff->loc[208] = &huff->nodeList[209];
  huff->nodeList[277].left = &huff->nodeList[210];
  huff->loc[209] = &huff->nodeList[210];
  huff->nodeList[301].left = &huff->nodeList[211];
  huff->loc[210] = &huff->nodeList[211];
  huff->nodeList[278].right = &huff->nodeList[212];
  huff->loc[211] = &huff->nodeList[212];
  huff->nodeList[263].left = &huff->nodeList[213];
  huff->loc[212] = &huff->nodeList[213];
  huff->nodeList[257].right = &huff->nodeList[214];
  huff->loc[213] = &huff->nodeList[214];
  huff->nodeList[286].right = &huff->nodeList[215];
  huff->loc[214] = &huff->nodeList[215];
  huff->nodeList[271].left = &huff->nodeList[216];
  huff->loc[215] = &huff->nodeList[216];
  huff->nodeList[381].right = &huff->nodeList[217];
  huff->loc[216] = &huff->nodeList[217];
  huff->nodeList[289].right = &huff->nodeList[218];
  huff->loc[217] = &huff->nodeList[218];
  huff->nodeList[262].right = &huff->nodeList[219];
  huff->loc[218] = &huff->nodeList[219];
  huff->nodeList[287].left = &huff->nodeList[220];
  huff->loc[219] = &huff->nodeList[220];
  huff->nodeList[299].right = &huff->nodeList[221];
  huff->loc[220] = &huff->nodeList[221];
  huff->nodeList[288].left = &huff->nodeList[222];
  huff->loc[221] = &huff->nodeList[222];
  huff->nodeList[334].right = &huff->nodeList[223];
  huff->loc[222] = &huff->nodeList[223];
  huff->nodeList[345].left = &huff->nodeList[224];
  huff->loc[223] = &huff->nodeList[224];
  huff->nodeList[403].left = &huff->nodeList[225];
  huff->loc[224] = &huff->nodeList[225];
  huff->nodeList[301].right = &huff->nodeList[226];
  huff->loc[225] = &huff->nodeList[226];
  huff->nodeList[307].left = &huff->nodeList[227];
  huff->loc[226] = &huff->nodeList[227];
  huff->nodeList[298].left = &huff->nodeList[228];
  huff->loc[227] = &huff->nodeList[228];
  huff->nodeList[310].left = &huff->nodeList[229];
  huff->loc[228] = &huff->nodeList[229];
  huff->nodeList[261].left = &huff->nodeList[230];
  huff->loc[229] = &huff->nodeList[230];
  huff->nodeList[272].left = &huff->nodeList[231];
  huff->loc[230] = &huff->nodeList[231];
  huff->nodeList[310].right = &huff->nodeList[232];
  huff->loc[231] = &huff->nodeList[232];
  huff->nodeList[327].right = &huff->nodeList[233];
  huff->loc[232] = &huff->nodeList[233];
  huff->nodeList[294].left = &huff->nodeList[234];
  huff->loc[233] = &huff->nodeList[234];
  huff->nodeList[286].left = &huff->nodeList[235];
  huff->loc[234] = &huff->nodeList[235];
  huff->nodeList[280].left = &huff->nodeList[236];
  huff->loc[235] = &huff->nodeList[236];
  huff->nodeList[315].right = &huff->nodeList[237];
  huff->loc[236] = &huff->nodeList[237];
  huff->nodeList[282].right = &huff->nodeList[238];
  huff->loc[237] = &huff->nodeList[238];
  huff->nodeList[324].right = &huff->nodeList[239];
  huff->loc[238] = &huff->nodeList[239];
  huff->nodeList[351].right = &huff->nodeList[240];
  huff->loc[239] = &huff->nodeList[240];
  huff->nodeList[384].right = &huff->nodeList[241];
  huff->loc[240] = &huff->nodeList[241];
  huff->nodeList[314].right = &huff->nodeList[242];
  huff->loc[241] = &huff->nodeList[242];
  huff->nodeList[293].right = &huff->nodeList[243];
  huff->loc[242] = &huff->nodeList[243];
  huff->nodeList[321].right = &huff->nodeList[244];
  huff->loc[243] = &huff->nodeList[244];
  huff->nodeList[337].right = &huff->nodeList[245];
  huff->loc[244] = &huff->nodeList[245];
  huff->nodeList[316].left = &huff->nodeList[246];
  huff->loc[245] = &huff->nodeList[246];
  huff->nodeList[318].right = &huff->nodeList[247];
  huff->loc[246] = &huff->nodeList[247];
  huff->nodeList[347].right = &huff->nodeList[248];
  huff->loc[247] = &huff->nodeList[248];
  huff->nodeList[372].left = &huff->nodeList[249];
  huff->loc[248] = &huff->nodeList[249];
  huff->nodeList[336].left = &huff->nodeList[250];
  huff->loc[249] = &huff->nodeList[250];
  huff->nodeList[374].right = &huff->nodeList[251];
  huff->loc[250] = &huff->nodeList[251];
  huff->nodeList[369].left = &huff->nodeList[252];
  huff->loc[251] = &huff->nodeList[252];
  huff->nodeList[379].left = &huff->nodeList[253];
  huff->loc[252] = &huff->nodeList[253];
  huff->nodeList[383].right = &huff->nodeList[254];
  huff->loc[253] = &huff->nodeList[254];
  huff->nodeList[393].right = &huff->nodeList[255];
  huff->loc[254] = &huff->nodeList[255];
  huff->nodeList[472].left = &huff->nodeList[256];
  huff->loc[255] = &huff->nodeList[256];
  
  
  huff->nodeList[257].left = huff->nodeList;
  
  huff->loc[256] = huff->nodeList;
  
  
  huff->nodeList[258].left = &huff->nodeList[257];
  huff->nodeList[0].parent = &huff->nodeList[257];
  huff->nodeList[0].symbol = 256;
  huff->nodeList[511].left = &huff->nodeList[508];
  huff->nodeList[1].parent = &huff->nodeList[508];
  huff->nodeList[1].weight = 274054;
  huff->nodeList[503].left = &huff->nodeList[493];
  huff->nodeList[2].parent = &huff->nodeList[493];
  huff->nodeList[2].weight = 68777;
  huff->nodeList[2].symbol = 1;
  huff->nodeList[496].right = &huff->nodeList[480];
  huff->nodeList[3].parent = &huff->nodeList[480];
  huff->nodeList[3].weight = 40460;
  huff->nodeList[3].symbol = 2;
  huff->nodeList[496].left = &huff->nodeList[479];
  huff->nodeList[4].parent = &huff->nodeList[479];
  huff->nodeList[4].weight = 40266;
  huff->nodeList[4].symbol = 3;
  huff->nodeList[498].right = &huff->nodeList[484];
  huff->nodeList[5].parent = &huff->nodeList[484];
  huff->nodeList[5].weight = 48059;
  huff->nodeList[5].symbol = 4;
  huff->nodeList[495].right = &huff->nodeList[478];
  huff->nodeList[6].parent = &huff->nodeList[478];
  huff->nodeList[6].weight = 39006;
  huff->nodeList[6].symbol = 5;
  huff->nodeList[499].left = &huff->nodeList[485];
  huff->nodeList[7].parent = &huff->nodeList[485];
  huff->nodeList[7].weight = 48630;
  huff->nodeList[7].symbol = 6;
  huff->nodeList[488].right = &huff->nodeList[465];
  huff->nodeList[8].parent = &huff->nodeList[465];
  huff->nodeList[8].weight = 27692;
  huff->nodeList[8].symbol = 7;
  huff->nodeList[474].right = &huff->nodeList[442];
  huff->nodeList[9].parent = &huff->nodeList[442];
  huff->nodeList[9].weight = 17712;
  huff->nodeList[9].symbol = 8;
  huff->nodeList[470].left = &huff->nodeList[434];
  huff->nodeList[10].parent = &huff->nodeList[434];
  huff->nodeList[10].weight = 15439;
  huff->nodeList[10].symbol = 9;
  huff->nodeList[459].right = &huff->nodeList[416];
  huff->nodeList[11].parent = &huff->nodeList[416];
  huff->nodeList[11].weight = 12386;
  huff->nodeList[11].symbol = 10;
  huff->nodeList[453].left = &huff->nodeList[403];
  huff->nodeList[12].parent = &huff->nodeList[403];
  huff->nodeList[12].weight = 10758;
  huff->nodeList[12].symbol = 11;
  huff->nodeList[447].right = &huff->nodeList[392];
  huff->nodeList[13].parent = &huff->nodeList[392];
  huff->nodeList[13].weight = 9420;
  huff->nodeList[13].symbol = 12;
  huff->nodeList[450].right = &huff->nodeList[398];
  huff->nodeList[14].parent = &huff->nodeList[398];
  huff->nodeList[14].weight = 9979;
  huff->nodeList[14].symbol = 13;
  huff->nodeList[445].right = &huff->nodeList[388];
  huff->nodeList[15].parent = &huff->nodeList[388];
  huff->nodeList[15].weight = 9346;
  huff->nodeList[15].symbol = 14;
  huff->nodeList[469].left = &huff->nodeList[432];
  huff->nodeList[16].parent = &huff->nodeList[432];
  huff->nodeList[16].weight = 15256;
  huff->nodeList[16].symbol = 15;
  huff->nodeList[462].left = &huff->nodeList[421];
  huff->nodeList[17].parent = &huff->nodeList[421];
  huff->nodeList[17].weight = 13184;
  huff->nodeList[17].symbol = 16;
  huff->nodeList[467].left = &huff->nodeList[429];
  huff->nodeList[18].parent = &huff->nodeList[429];
  huff->nodeList[18].weight = 14319;
  huff->nodeList[18].symbol = 17;
  huff->nodeList[435].left = &huff->nodeList[369];
  huff->nodeList[19].parent = &huff->nodeList[369];
  huff->nodeList[19].weight = 7750;
  huff->nodeList[19].symbol = 18;
  huff->nodeList[430].right = &huff->nodeList[363];
  huff->nodeList[20].parent = &huff->nodeList[363];
  huff->nodeList[20].weight = 7221;
  huff->nodeList[20].symbol = 19;
  huff->nodeList[414].left = &huff->nodeList[340];
  huff->nodeList[21].parent = &huff->nodeList[340];
  huff->nodeList[21].weight = 6095;
  huff->nodeList[21].symbol = 20;
  huff->nodeList[406].right = &huff->nodeList[330];
  huff->nodeList[22].parent = &huff->nodeList[330];
  huff->nodeList[22].weight = 5666;
  huff->nodeList[22].symbol = 21;
  huff->nodeList[460].left = &huff->nodeList[417];
  huff->nodeList[23].parent = &huff->nodeList[417];
  huff->nodeList[23].weight = 12606;
  huff->nodeList[23].symbol = 22;
  huff->nodeList[24].parent = &huff->nodeList[363];
  huff->nodeList[24].weight = 7263;
  huff->nodeList[24].symbol = 23;
  huff->nodeList[431].left = &huff->nodeList[364];
  huff->nodeList[25].parent = &huff->nodeList[364];
  huff->nodeList[25].weight = 7322;
  huff->nodeList[25].symbol = 24;
  huff->nodeList[409].left = &huff->nodeList[333];
  huff->nodeList[26].parent = &huff->nodeList[333];
  huff->nodeList[26].weight = 5807;
  huff->nodeList[26].symbol = 25;
  huff->nodeList[455].right = &huff->nodeList[408];
  huff->nodeList[27].parent = &huff->nodeList[408];
  huff->nodeList[27].weight = 11628;
  huff->nodeList[27].symbol = 26;
  huff->nodeList[416].right = &huff->nodeList[344];
  huff->nodeList[28].parent = &huff->nodeList[344];
  huff->nodeList[28].weight = 6199;
  huff->nodeList[28].symbol = 27;
  huff->nodeList[436].left = &huff->nodeList[371];
  huff->nodeList[29].parent = &huff->nodeList[371];
  huff->nodeList[29].weight = 7826;
  huff->nodeList[29].symbol = 28;
  huff->nodeList[418].right = &huff->nodeList[347];
  huff->nodeList[30].parent = &huff->nodeList[347];
  huff->nodeList[30].weight = 6349;
  huff->nodeList[30].symbol = 29;
  huff->nodeList[434].left = &huff->nodeList[368];
  huff->nodeList[31].parent = &huff->nodeList[368];
  huff->nodeList[31].weight = 7698;
  huff->nodeList[31].symbol = 30;
  huff->nodeList[448].right = &huff->nodeList[394];
  huff->nodeList[32].parent = &huff->nodeList[394];
  huff->nodeList[32].weight = 9656;
  huff->nodeList[32].symbol = 31;
  huff->nodeList[490].left = &huff->nodeList[468];
  huff->nodeList[33].parent = &huff->nodeList[468];
  huff->nodeList[33].weight = 28968;
  huff->nodeList[33].symbol = 32;
  huff->nodeList[399].right = &huff->nodeList[321];
  huff->nodeList[34].parent = &huff->nodeList[321];
  huff->nodeList[34].weight = 5164;
  huff->nodeList[34].symbol = 33;
  huff->nodeList[463].right = &huff->nodeList[423];
  huff->nodeList[35].parent = &huff->nodeList[423];
  huff->nodeList[35].weight = 13629;
  huff->nodeList[35].symbol = 34;
  huff->nodeList[36].parent = huff->nodeList[414].left;
  huff->nodeList[36].weight = 6058;
  huff->nodeList[36].symbol = 35;
  huff->nodeList[392].right = &huff->nodeList[312];
  huff->nodeList[37].parent = &huff->nodeList[312];
  huff->nodeList[37].weight = 4745;
  huff->nodeList[37].symbol = 36;
  huff->nodeList[385].left = &huff->nodeList[303];
  huff->nodeList[38].parent = &huff->nodeList[303];
  huff->nodeList[38].weight = 4519;
  huff->nodeList[38].symbol = 37;
  huff->nodeList[400].left = &huff->nodeList[322];
  huff->nodeList[39].parent = &huff->nodeList[322];
  huff->nodeList[39].weight = 5199;
  huff->nodeList[39].symbol = 38;
  huff->nodeList[393].left = &huff->nodeList[313];
  huff->nodeList[40].parent = &huff->nodeList[313];
  huff->nodeList[40].weight = 4807;
  huff->nodeList[40].symbol = 39;
  huff->nodeList[402].right = &huff->nodeList[326];
  huff->nodeList[41].parent = &huff->nodeList[326];
  huff->nodeList[41].weight = 5323;
  huff->nodeList[41].symbol = 40;
  huff->nodeList[355].left = &huff->nodeList[275];
  huff->nodeList[42].parent = &huff->nodeList[275];
  huff->nodeList[42].weight = 3433;
  huff->nodeList[42].symbol = 41;
  huff->nodeList[355].right = &huff->nodeList[276];
  huff->nodeList[43].parent = &huff->nodeList[276];
  huff->nodeList[43].weight = 3455;
  huff->nodeList[43].symbol = 42;
  huff->nodeList[360].right = &huff->nodeList[282];
  huff->nodeList[44].parent = &huff->nodeList[282];
  huff->nodeList[44].weight = 3563;
  huff->nodeList[44].symbol = 43;
  huff->nodeList[426].left = &huff->nodeList[357];
  huff->nodeList[45].parent = &huff->nodeList[357];
  huff->nodeList[45].weight = 6979;
  huff->nodeList[45].symbol = 44;
  huff->nodeList[401].left = &huff->nodeList[324];
  huff->nodeList[46].parent = &huff->nodeList[324];
  huff->nodeList[46].weight = 5229;
  huff->nodeList[46].symbol = 45;
  huff->nodeList[398].right = &huff->nodeList[320];
  huff->nodeList[47].parent = &huff->nodeList[320];
  huff->nodeList[47].weight = 5002;
  huff->nodeList[47].symbol = 46;
  huff->nodeList[382].left = &huff->nodeList[300];
  huff->nodeList[48].parent = &huff->nodeList[300];
  huff->nodeList[48].weight = 4423;
  huff->nodeList[48].symbol = 47;
  huff->nodeList[466].left = &huff->nodeList[427];
  huff->nodeList[49].parent = &huff->nodeList[427];
  huff->nodeList[49].weight = 14108;
  huff->nodeList[49].symbol = 48;
  huff->nodeList[464].left = &huff->nodeList[424];
  huff->nodeList[50].parent = &huff->nodeList[424];
  huff->nodeList[50].weight = 13631;
  huff->nodeList[50].symbol = 49;
  huff->nodeList[457].left = &huff->nodeList[411];
  huff->nodeList[51].parent = &huff->nodeList[411];
  huff->nodeList[51].weight = 11908;
  huff->nodeList[51].symbol = 50;
  huff->nodeList[52].parent = &huff->nodeList[411];
  huff->nodeList[52].weight = 11801;
  huff->nodeList[52].symbol = 51;
  huff->nodeList[451].left = &huff->nodeList[399];
  huff->nodeList[53].parent = &huff->nodeList[399];
  huff->nodeList[53].weight = 10261;
  huff->nodeList[53].symbol = 52;
  huff->nodeList[433].left = &huff->nodeList[367];
  huff->nodeList[54].parent = &huff->nodeList[367];
  huff->nodeList[54].weight = 7635;
  huff->nodeList[54].symbol = 53;
  huff->nodeList[430].left = &huff->nodeList[362];
  huff->nodeList[55].parent = &huff->nodeList[362];
  huff->nodeList[55].weight = 7215;
  huff->nodeList[55].symbol = 54;
  huff->nodeList[56].parent = &huff->nodeList[362];
  huff->nodeList[56].weight = 7218;
  huff->nodeList[56].symbol = 55;
  huff->nodeList[446].left = &huff->nodeList[389];
  huff->nodeList[57].parent = &huff->nodeList[389];
  huff->nodeList[57].weight = 9353;
  huff->nodeList[57].symbol = 56;
  huff->nodeList[415].right = &huff->nodeList[343];
  huff->nodeList[58].parent = &huff->nodeList[343];
  huff->nodeList[58].weight = 6161;
  huff->nodeList[58].symbol = 57;
  huff->nodeList[407].left = &huff->nodeList[331];
  huff->nodeList[59].parent = &huff->nodeList[331];
  huff->nodeList[59].weight = 5689;
  huff->nodeList[59].symbol = 58;
  huff->nodeList[388].left = &huff->nodeList[308];
  huff->nodeList[60].parent = &huff->nodeList[308];
  huff->nodeList[60].weight = 4649;
  huff->nodeList[60].symbol = 59;
  huff->nodeList[61].parent = huff->nodeList[398].right;
  huff->nodeList[61].weight = 5026;
  huff->nodeList[61].symbol = 60;
  huff->nodeList[409].right = &huff->nodeList[334];
  huff->nodeList[62].parent = &huff->nodeList[334];
  huff->nodeList[62].weight = 5866;
  huff->nodeList[62].symbol = 61;
  huff->nodeList[437].left = &huff->nodeList[373];
  huff->nodeList[63].parent = &huff->nodeList[373];
  huff->nodeList[63].weight = 8002;
  huff->nodeList[63].symbol = 62;
  huff->nodeList[452].left = &huff->nodeList[401];
  huff->nodeList[64].parent = &huff->nodeList[401];
  huff->nodeList[64].weight = 10534;
  huff->nodeList[64].symbol = 63;
  huff->nodeList[469].right = &huff->nodeList[433];
  huff->nodeList[65].parent = &huff->nodeList[433];
  huff->nodeList[65].weight = 15381;
  huff->nodeList[65].symbol = 64;
  huff->nodeList[443].left = &huff->nodeList[383];
  huff->nodeList[66].parent = &huff->nodeList[383];
  huff->nodeList[66].weight = 8874;
  huff->nodeList[66].symbol = 65;
  huff->nodeList[456].right = &huff->nodeList[410];
  huff->nodeList[67].parent = &huff->nodeList[410];
  huff->nodeList[67].weight = 11798;
  huff->nodeList[67].symbol = 66;
  huff->nodeList[429].right = &huff->nodeList[361];
  huff->nodeList[68].parent = &huff->nodeList[361];
  huff->nodeList[68].weight = 7199;
  huff->nodeList[68].symbol = 67;
  huff->nodeList[461].left = &huff->nodeList[419];
  huff->nodeList[69].parent = &huff->nodeList[419];
  huff->nodeList[69].weight = 12814;
  huff->nodeList[69].symbol = 68;
  huff->nodeList[415].left = &huff->nodeList[342];
  huff->nodeList[70].parent = &huff->nodeList[342];
  huff->nodeList[70].weight = 6103;
  huff->nodeList[70].symbol = 69;
  huff->nodeList[397].left = &huff->nodeList[318];
  huff->nodeList[71].parent = &huff->nodeList[318];
  huff->nodeList[71].weight = 4982;
  huff->nodeList[71].symbol = 70;
  huff->nodeList[412].right = &huff->nodeList[337];
  huff->nodeList[72].parent = &huff->nodeList[337];
  huff->nodeList[72].weight = 5972;
  huff->nodeList[72].symbol = 71;
  huff->nodeList[422].right = &huff->nodeList[353];
  huff->nodeList[73].parent = &huff->nodeList[353];
  huff->nodeList[73].weight = 6779;
  huff->nodeList[73].symbol = 72;
  huff->nodeList[396].left = &huff->nodeList[316];
  huff->nodeList[74].parent = &huff->nodeList[316];
  huff->nodeList[74].weight = 4929;
  huff->nodeList[74].symbol = 73;
  huff->nodeList[75].parent = huff->nodeList[402].right;
  huff->nodeList[75].weight = 5333;
  huff->nodeList[75].symbol = 74;
  huff->nodeList[358].left = &huff->nodeList[279];
  huff->nodeList[76].parent = &huff->nodeList[279];
  huff->nodeList[76].weight = 3503;
  huff->nodeList[76].symbol = 75;
  huff->nodeList[381].left = &huff->nodeList[299];
  huff->nodeList[77].parent = &huff->nodeList[299];
  huff->nodeList[77].weight = 4345;
  huff->nodeList[77].symbol = 76;
  huff->nodeList[414].right = &huff->nodeList[341];
  huff->nodeList[78].parent = &huff->nodeList[341];
  huff->nodeList[78].weight = 6098;
  huff->nodeList[78].symbol = 77;
  huff->nodeList[79].parent = huff->nodeList[466].left;
  huff->nodeList[79].weight = 14117;
  huff->nodeList[79].symbol = 78;
  huff->nodeList[472].right = &huff->nodeList[438];
  huff->nodeList[80].parent = &huff->nodeList[438];
  huff->nodeList[80].weight = 16440;
  huff->nodeList[80].symbol = 79;
  huff->nodeList[420].left = &huff->nodeList[349];
  huff->nodeList[81].parent = &huff->nodeList[349];
  huff->nodeList[81].weight = 6446;
  huff->nodeList[81].symbol = 80;
  huff->nodeList[342].right = &huff->nodeList[265];
  huff->nodeList[82].parent = &huff->nodeList[265];
  huff->nodeList[82].weight = 3062;
  huff->nodeList[82].symbol = 81;
  huff->nodeList[391].right = &huff->nodeList[311];
  huff->nodeList[83].parent = &huff->nodeList[311];
  huff->nodeList[83].weight = 4695;
  huff->nodeList[83].symbol = 82;
  huff->nodeList[84].parent = &huff->nodeList[265];
  huff->nodeList[84].weight = 3085;
  huff->nodeList[84].symbol = 83;
  huff->nodeList[378].left = &huff->nodeList[296];
  huff->nodeList[85].parent = &huff->nodeList[296];
  huff->nodeList[85].weight = 4198;
  huff->nodeList[85].symbol = 84;
  huff->nodeList[374].left = &huff->nodeList[291];
  huff->nodeList[86].parent = &huff->nodeList[291];
  huff->nodeList[86].weight = 4013;
  huff->nodeList[86].symbol = 85;
  huff->nodeList[370].left = &huff->nodeList[287];
  huff->nodeList[87].parent = &huff->nodeList[287];
  huff->nodeList[87].weight = 3878;
  huff->nodeList[87].symbol = 86;
  huff->nodeList[354].right = &huff->nodeList[274];
  huff->nodeList[88].parent = &huff->nodeList[274];
  huff->nodeList[88].weight = 3414;
  huff->nodeList[88].symbol = 87;
  huff->nodeList[404].right = &huff->nodeList[328];
  huff->nodeList[89].parent = &huff->nodeList[328];
  huff->nodeList[89].weight = 5514;
  huff->nodeList[89].symbol = 88;
  huff->nodeList[375].left = &huff->nodeList[292];
  huff->nodeList[90].parent = &huff->nodeList[292];
  huff->nodeList[90].weight = 4092;
  huff->nodeList[90].symbol = 89;
  huff->nodeList[349].right = &huff->nodeList[269];
  huff->nodeList[91].parent = &huff->nodeList[269];
  huff->nodeList[91].weight = 3261;
  huff->nodeList[91].symbol = 90;
  huff->nodeList[92].parent = huff->nodeList[392].right;
  huff->nodeList[92].weight = 4740;
  huff->nodeList[92].symbol = 91;
  huff->nodeList[385].right = &huff->nodeList[304];
  huff->nodeList[93].parent = &huff->nodeList[304];
  huff->nodeList[93].weight = 4544;
  huff->nodeList[93].symbol = 92;
  huff->nodeList[345].right = &huff->nodeList[267];
  huff->nodeList[94].parent = &huff->nodeList[267];
  huff->nodeList[94].weight = 3127;
  huff->nodeList[94].symbol = 93;
  huff->nodeList[353].left = &huff->nodeList[272];
  huff->nodeList[95].parent = &huff->nodeList[272];
  huff->nodeList[95].weight = 3385;
  huff->nodeList[95].symbol = 94;
  huff->nodeList[96].parent = huff->nodeList[433].left;
  huff->nodeList[96].weight = 7688;
  huff->nodeList[96].symbol = 95;
  huff->nodeList[454].left = &huff->nodeList[405];
  huff->nodeList[97].parent = &huff->nodeList[405];
  huff->nodeList[97].weight = 11126;
  huff->nodeList[97].symbol = 96;
  huff->nodeList[419].right = &huff->nodeList[348];
  huff->nodeList[98].parent = &huff->nodeList[348];
  huff->nodeList[98].weight = 6417;
  huff->nodeList[98].symbol = 97;
  huff->nodeList[402].left = &huff->nodeList[325];
  huff->nodeList[99].parent = &huff->nodeList[325];
  huff->nodeList[99].weight = 5297;
  huff->nodeList[99].symbol = 98;
  huff->nodeList[100].parent = huff->nodeList[385].left;
  huff->nodeList[100].weight = 4529;
  huff->nodeList[100].symbol = 99;
  huff->nodeList[418].left = &huff->nodeList[346];
  huff->nodeList[101].parent = &huff->nodeList[346];
  huff->nodeList[101].weight = 6333;
  huff->nodeList[101].symbol = 100;
  huff->nodeList[102].parent = huff->nodeList[378].left;
  huff->nodeList[102].weight = 4210;
  huff->nodeList[102].symbol = 101;
  huff->nodeList[428].left = &huff->nodeList[359];
  huff->nodeList[103].parent = &huff->nodeList[359];
  huff->nodeList[103].weight = 7056;
  huff->nodeList[103].symbol = 102;
  huff->nodeList[104].parent = huff->nodeList[388].left;
  huff->nodeList[104].weight = 4658;
  huff->nodeList[104].symbol = 103;
  huff->nodeList[105].parent = huff->nodeList[415].right;
  huff->nodeList[105].weight = 6190;
  huff->nodeList[105].symbol = 104;
  huff->nodeList[106].parent = huff->nodeList[358].left;
  huff->nodeList[106].weight = 3512;
  huff->nodeList[106].symbol = 105;
  huff->nodeList[329].left = &huff->nodeList[260];
  huff->nodeList[107].parent = &huff->nodeList[260];
  huff->nodeList[107].weight = 2843;
  huff->nodeList[107].symbol = 106;
  huff->nodeList[357].left = &huff->nodeList[278];
  huff->nodeList[108].parent = &huff->nodeList[278];
  huff->nodeList[108].weight = 3479;
  huff->nodeList[108].symbol = 107;
  huff->nodeList[447].left = &huff->nodeList[391];
  huff->nodeList[109].parent = &huff->nodeList[391];
  huff->nodeList[109].weight = 9369;
  huff->nodeList[109].symbol = 108;
  huff->nodeList[400].right = &huff->nodeList[323];
  huff->nodeList[110].parent = &huff->nodeList[323];
  huff->nodeList[110].weight = 5203;
  huff->nodeList[110].symbol = 109;
  huff->nodeList[396].right = &huff->nodeList[317];
  huff->nodeList[111].parent = &huff->nodeList[317];
  huff->nodeList[111].weight = 4980;
  huff->nodeList[111].symbol = 110;
  huff->nodeList[410].left = &huff->nodeList[335];
  huff->nodeList[112].parent = &huff->nodeList[335];
  huff->nodeList[112].weight = 5881;
  huff->nodeList[112].symbol = 111;
  huff->nodeList[431].right = &huff->nodeList[365];
  huff->nodeList[113].parent = &huff->nodeList[365];
  huff->nodeList[113].weight = 7509;
  huff->nodeList[113].symbol = 112;
  huff->nodeList[378].right = &huff->nodeList[297];
  huff->nodeList[114].parent = &huff->nodeList[297];
  huff->nodeList[114].weight = 4292;
  huff->nodeList[114].symbol = 113;
  huff->nodeList[115].parent = huff->nodeList[414].right;
  huff->nodeList[115].weight = 6097;
  huff->nodeList[115].symbol = 114;
  huff->nodeList[116].parent = huff->nodeList[404].right;
  huff->nodeList[116].weight = 5492;
  huff->nodeList[116].symbol = 115;
  huff->nodeList[387].right = &huff->nodeList[307];
  huff->nodeList[117].parent = &huff->nodeList[307];
  huff->nodeList[117].weight = 4648;
  huff->nodeList[117].symbol = 116;
  huff->nodeList[338].left = &huff->nodeList[264];
  huff->nodeList[118].parent = &huff->nodeList[264];
  huff->nodeList[118].weight = 2996;
  huff->nodeList[118].symbol = 117;
  huff->nodeList[397].right = &huff->nodeList[319];
  huff->nodeList[119].parent = &huff->nodeList[319];
  huff->nodeList[119].weight = 4988;
  huff->nodeList[119].symbol = 118;
  huff->nodeList[377].left = &huff->nodeList[294];
  huff->nodeList[120].parent = &huff->nodeList[294];
  huff->nodeList[120].weight = 4163;
  huff->nodeList[120].symbol = 119;
  huff->nodeList[420].right = &huff->nodeList[350];
  huff->nodeList[121].parent = &huff->nodeList[350];
  huff->nodeList[121].weight = 6534;
  huff->nodeList[121].symbol = 120;
  huff->nodeList[372].right = &huff->nodeList[290];
  huff->nodeList[122].parent = &huff->nodeList[290];
  huff->nodeList[122].weight = 4001;
  huff->nodeList[122].symbol = 121;
  huff->nodeList[379].right = &huff->nodeList[298];
  huff->nodeList[123].parent = &huff->nodeList[298];
  huff->nodeList[123].weight = 4342;
  huff->nodeList[123].symbol = 122;
  huff->nodeList[384].left = &huff->nodeList[302];
  huff->nodeList[124].parent = &huff->nodeList[302];
  huff->nodeList[124].weight = 4488;
  huff->nodeList[124].symbol = 123;
  huff->nodeList[413].right = &huff->nodeList[339];
  huff->nodeList[125].parent = &huff->nodeList[339];
  huff->nodeList[125].weight = 6039;
  huff->nodeList[125].symbol = 124;
  huff->nodeList[395].left = &huff->nodeList[314];
  huff->nodeList[126].parent = &huff->nodeList[314];
  huff->nodeList[126].weight = 4827;
  huff->nodeList[126].symbol = 125;
  huff->nodeList[428].right = &huff->nodeList[360];
  huff->nodeList[127].parent = &huff->nodeList[360];
  huff->nodeList[127].weight = 7112;
  huff->nodeList[127].symbol = 126;
  huff->nodeList[441].left = &huff->nodeList[380];
  huff->nodeList[128].parent = &huff->nodeList[380];
  huff->nodeList[128].weight = 8654;
  huff->nodeList[128].symbol = 127;
  huff->nodeList[487].left = &huff->nodeList[462];
  huff->nodeList[129].parent = &huff->nodeList[462];
  huff->nodeList[129].weight = 26712;
  huff->nodeList[129].symbol = 128;
  huff->nodeList[130].parent = &huff->nodeList[380];
  huff->nodeList[130].weight = 8688;
  huff->nodeList[130].symbol = 129;
  huff->nodeList[131].parent = huff->nodeList[448].right;
  huff->nodeList[131].weight = 9677;
  huff->nodeList[131].symbol = 130;
  huff->nodeList[446].right = &huff->nodeList[390];
  huff->nodeList[132].parent = &huff->nodeList[390];
  huff->nodeList[132].weight = 9368;
  huff->nodeList[132].symbol = 131;
  huff->nodeList[133].parent = huff->nodeList[429].right;
  huff->nodeList[133].weight = 7209;
  huff->nodeList[133].symbol = 132;
  huff->nodeList[354].left = &huff->nodeList[273];
  huff->nodeList[134].parent = &huff->nodeList[273];
  huff->nodeList[134].weight = 3399;
  huff->nodeList[134].symbol = 133;
  huff->nodeList[135].parent = huff->nodeList[384].left;
  huff->nodeList[135].weight = 4473;
  huff->nodeList[135].symbol = 134;
  huff->nodeList[389].right = &huff->nodeList[309];
  huff->nodeList[136].parent = &huff->nodeList[309];
  huff->nodeList[136].weight = 4677;
  huff->nodeList[136].symbol = 135;
  huff->nodeList[137].parent = huff->nodeList[454].left;
  huff->nodeList[137].weight = 11087;
  huff->nodeList[137].symbol = 136;
  huff->nodeList[375].right = &huff->nodeList[293];
  huff->nodeList[138].parent = &huff->nodeList[293];
  huff->nodeList[138].weight = 4094;
  huff->nodeList[138].symbol = 137;
  huff->nodeList[139].parent = &huff->nodeList[273];
  huff->nodeList[139].weight = 3404;
  huff->nodeList[139].symbol = 138;
  huff->nodeList[377].right = &huff->nodeList[295];
  huff->nodeList[140].parent = &huff->nodeList[295];
  huff->nodeList[140].weight = 4176;
  huff->nodeList[140].symbol = 139;
  huff->nodeList[422].left = &huff->nodeList[352];
  huff->nodeList[141].parent = &huff->nodeList[352];
  huff->nodeList[141].weight = 6733;
  huff->nodeList[141].symbol = 140;
  huff->nodeList[365].left = &huff->nodeList[284];
  huff->nodeList[142].parent = &huff->nodeList[284];
  huff->nodeList[142].weight = 3702;
  huff->nodeList[142].symbol = 141;
  huff->nodeList[455].left = &huff->nodeList[407];
  huff->nodeList[143].parent = &huff->nodeList[407];
  huff->nodeList[143].weight = 11420;
  huff->nodeList[143].symbol = 142;
  huff->nodeList[395].right = &huff->nodeList[315];
  huff->nodeList[144].parent = &huff->nodeList[315];
  huff->nodeList[144].weight = 4867;
  huff->nodeList[144].symbol = 143;
  huff->nodeList[412].left = &huff->nodeList[336];
  huff->nodeList[145].parent = &huff->nodeList[336];
  huff->nodeList[145].weight = 5968;
  huff->nodeList[145].symbol = 144;
  huff->nodeList[356].left = &huff->nodeList[277];
  huff->nodeList[146].parent = &huff->nodeList[277];
  huff->nodeList[146].weight = 3475;
  huff->nodeList[146].symbol = 145;
  huff->nodeList[147].parent = &huff->nodeList[284];
  huff->nodeList[147].weight = 3722;
  huff->nodeList[147].symbol = 146;
  huff->nodeList[359].right = &huff->nodeList[281];
  huff->nodeList[148].parent = &huff->nodeList[281];
  huff->nodeList[148].weight = 3560;
  huff->nodeList[148].symbol = 147;
  huff->nodeList[386].left = &huff->nodeList[305];
  huff->nodeList[149].parent = &huff->nodeList[305];
  huff->nodeList[149].weight = 4571;
  huff->nodeList[149].symbol = 148;
  huff->nodeList[150].parent = huff->nodeList[329].left;
  huff->nodeList[150].weight = 2720;
  huff->nodeList[150].symbol = 149;
  huff->nodeList[151].parent = huff->nodeList[349].right;
  huff->nodeList[151].weight = 3189;
  huff->nodeList[151].symbol = 150;
  huff->nodeList[344].right = &huff->nodeList[266];
  huff->nodeList[152].parent = &huff->nodeList[266];
  huff->nodeList[152].weight = 3099;
  huff->nodeList[152].symbol = 151;
  huff->nodeList[387].left = &huff->nodeList[306];
  huff->nodeList[153].parent = &huff->nodeList[306];
  huff->nodeList[153].weight = 4595;
  huff->nodeList[153].symbol = 152;
  huff->nodeList[154].parent = huff->nodeList[374].left;
  huff->nodeList[154].weight = 4044;
  huff->nodeList[154].symbol = 153;
  huff->nodeList[155].parent = huff->nodeList[382].left;
  huff->nodeList[155].weight = 4402;
  huff->nodeList[155].symbol = 154;
  huff->nodeList[371].left = &huff->nodeList[289];
  huff->nodeList[156].parent = &huff->nodeList[289];
  huff->nodeList[156].weight = 3889;
  huff->nodeList[156].symbol = 155;
  huff->nodeList[157].parent = huff->nodeList[397].right;
  huff->nodeList[157].weight = 4989;
  huff->nodeList[157].symbol = 156;
  huff->nodeList[346].right = &huff->nodeList[268];
  huff->nodeList[158].parent = &huff->nodeList[268];
  huff->nodeList[158].weight = 3186;
  huff->nodeList[158].symbol = 157;
  huff->nodeList[159].parent = &huff->nodeList[268];
  huff->nodeList[159].weight = 3153;
  huff->nodeList[159].symbol = 158;
  huff->nodeList[404].left = &huff->nodeList[327];
  huff->nodeList[160].parent = &huff->nodeList[327];
  huff->nodeList[160].weight = 5387;
  huff->nodeList[160].symbol = 159;
  huff->nodeList[161].parent = huff->nodeList[437].left;
  huff->nodeList[161].weight = 8020;
  huff->nodeList[161].symbol = 160;
  huff->nodeList[352].left = &huff->nodeList[271];
  huff->nodeList[162].parent = &huff->nodeList[271];
  huff->nodeList[162].weight = 3322;
  huff->nodeList[162].symbol = 161;
  huff->nodeList[366].right = &huff->nodeList[285];
  huff->nodeList[163].parent = &huff->nodeList[285];
  huff->nodeList[163].weight = 3775;
  huff->nodeList[163].symbol = 162;
  huff->nodeList[332].right = &huff->nodeList[262];
  huff->nodeList[164].parent = &huff->nodeList[262];
  huff->nodeList[164].weight = 2886;
  huff->nodeList[164].symbol = 163;
  huff->nodeList[165].parent = huff->nodeList[377].right;
  huff->nodeList[165].weight = 4191;
  huff->nodeList[165].symbol = 164;
  huff->nodeList[332].left = &huff->nodeList[261];
  huff->nodeList[166].parent = &huff->nodeList[261];
  huff->nodeList[166].weight = 2879;
  huff->nodeList[166].symbol = 165;
  huff->nodeList[167].parent = huff->nodeList[344].right;
  huff->nodeList[167].weight = 3110;
  huff->nodeList[167].symbol = 166;
  huff->nodeList[325].left = &huff->nodeList[259];
  huff->nodeList[168].parent = &huff->nodeList[259];
  huff->nodeList[168].weight = 2576;
  huff->nodeList[168].symbol = 167;
  huff->nodeList[364].right = &huff->nodeList[283];
  huff->nodeList[169].parent = &huff->nodeList[283];
  huff->nodeList[169].weight = 3693;
  huff->nodeList[169].symbol = 168;
  huff->nodeList[170].parent = &huff->nodeList[258];
  huff->nodeList[170].weight = 2436;
  huff->nodeList[170].symbol = 169;
  huff->nodeList[171].parent = huff->nodeList[396].right;
  huff->nodeList[171].weight = 4935;
  huff->nodeList[171].symbol = 170;
  huff->nodeList[172].parent = huff->nodeList[338].left;
  huff->nodeList[172].weight = 3017;
  huff->nodeList[172].symbol = 171;
  huff->nodeList[173].parent = huff->nodeList[359].right;
  huff->nodeList[173].weight = 3538;
  huff->nodeList[173].symbol = 172;
  huff->nodeList[174].parent = huff->nodeList[407].left;
  huff->nodeList[174].weight = 5688;
  huff->nodeList[174].symbol = 173;
  huff->nodeList[175].parent = huff->nodeList[355].right;
  huff->nodeList[175].weight = 3444;
  huff->nodeList[175].symbol = 174;
  huff->nodeList[176].parent = huff->nodeList[354].right;
  huff->nodeList[176].weight = 3410;
  huff->nodeList[176].symbol = 175;
  huff->nodeList[444].right = &huff->nodeList[386];
  huff->nodeList[177].parent = &huff->nodeList[386];
  huff->nodeList[177].weight = 9170;
  huff->nodeList[177].symbol = 176;
  huff->nodeList[178].parent = huff->nodeList[391].right;
  huff->nodeList[178].weight = 4708;
  huff->nodeList[178].symbol = 177;
  huff->nodeList[179].parent = huff->nodeList[355].left;
  huff->nodeList[179].weight = 3425;
  huff->nodeList[179].symbol = 178;
  huff->nodeList[351].left = &huff->nodeList[270];
  huff->nodeList[180].parent = &huff->nodeList[270];
  huff->nodeList[180].weight = 3273;
  huff->nodeList[180].symbol = 179;
  huff->nodeList[181].parent = huff->nodeList[364].right;
  huff->nodeList[181].weight = 3684;
  huff->nodeList[181].symbol = 180;
  huff->nodeList[182].parent = huff->nodeList[385].right;
  huff->nodeList[182].weight = 4564;
  huff->nodeList[182].symbol = 181;
  huff->nodeList[425].right = &huff->nodeList[356];
  huff->nodeList[183].parent = &huff->nodeList[356];
  huff->nodeList[183].weight = 6957;
  huff->nodeList[183].symbol = 182;
  huff->nodeList[184].parent = huff->nodeList[393].left;
  huff->nodeList[184].weight = 4817;
  huff->nodeList[184].symbol = 183;
  huff->nodeList[185].parent = huff->nodeList[400].right;
  huff->nodeList[185].weight = 5224;
  huff->nodeList[185].symbol = 184;
  huff->nodeList[186].parent = huff->nodeList[351].left;
  huff->nodeList[186].weight = 3285;
  huff->nodeList[186].symbol = 185;
  huff->nodeList[187].parent = huff->nodeList[345].right;
  huff->nodeList[187].weight = 3143;
  huff->nodeList[187].symbol = 186;
  huff->nodeList[188].parent = huff->nodeList[378].right;
  huff->nodeList[188].weight = 4227;
  huff->nodeList[188].symbol = 187;
  huff->nodeList[189].parent = huff->nodeList[406].right;
  huff->nodeList[189].weight = 5630;
  huff->nodeList[189].symbol = 188;
  huff->nodeList[190].parent = huff->nodeList[413].right;
  huff->nodeList[190].weight = 6053;
  huff->nodeList[190].symbol = 189;
  huff->nodeList[191].parent = huff->nodeList[409].left;
  huff->nodeList[191].weight = 5851;
  huff->nodeList[191].symbol = 190;
  huff->nodeList[192].parent = huff->nodeList[420].right;
  huff->nodeList[192].weight = 6507;
  huff->nodeList[192].symbol = 191;
  huff->nodeList[193].parent = huff->nodeList[464].left;
  huff->nodeList[193].weight = 13692;
  huff->nodeList[193].symbol = 192;
  huff->nodeList[439].left = &huff->nodeList[376];
  huff->nodeList[194].parent = &huff->nodeList[376];
  huff->nodeList[194].weight = 8270;
  huff->nodeList[194].symbol = 193;
  huff->nodeList[195].parent = &huff->nodeList[376];
  huff->nodeList[195].weight = 8260;
  huff->nodeList[195].symbol = 194;
  huff->nodeList[406].left = &huff->nodeList[329];
  huff->nodeList[196].parent = &huff->nodeList[329];
  huff->nodeList[196].weight = 5583;
  huff->nodeList[196].symbol = 195;
  huff->nodeList[432].left = &huff->nodeList[366];
  huff->nodeList[197].parent = &huff->nodeList[366];
  huff->nodeList[197].weight = 7568;
  huff->nodeList[197].symbol = 196;
  huff->nodeList[198].parent = huff->nodeList[375].left;
  huff->nodeList[198].weight = 4082;
  huff->nodeList[198].symbol = 197;
  huff->nodeList[199].parent = huff->nodeList[372].right;
  huff->nodeList[199].weight = 3984;
  huff->nodeList[199].symbol = 198;
  huff->nodeList[200].parent = huff->nodeList[386].left;
  huff->nodeList[200].weight = 4574;
  huff->nodeList[200].symbol = 199;
  huff->nodeList[201].parent = huff->nodeList[419].right;
  huff->nodeList[201].weight = 6440;
  huff->nodeList[201].symbol = 200;
  huff->nodeList[358].right = &huff->nodeList[280];
  huff->nodeList[202].parent = &huff->nodeList[280];
  huff->nodeList[202].weight = 3533;
  huff->nodeList[202].symbol = 201;
  huff->nodeList[335].right = &huff->nodeList[263];
  huff->nodeList[203].parent = &huff->nodeList[263];
  huff->nodeList[203].weight = 2992;
  huff->nodeList[203].symbol = 202;
  huff->nodeList[204].parent = huff->nodeList[325].left;
  huff->nodeList[204].weight = 2708;
  huff->nodeList[204].symbol = 203;
  huff->nodeList[205].parent = huff->nodeList[400].left;
  huff->nodeList[205].weight = 5190;
  huff->nodeList[205].symbol = 204;
  huff->nodeList[370].right = &huff->nodeList[288];
  huff->nodeList[206].parent = &huff->nodeList[288];
  huff->nodeList[206].weight = 3889;
  huff->nodeList[206].symbol = 205;
  huff->nodeList[207].parent = huff->nodeList[366].right;
  huff->nodeList[207].weight = 3799;
  huff->nodeList[207].symbol = 206;
  huff->nodeList[208].parent = huff->nodeList[387].left;
  huff->nodeList[208].weight = 4582;
  huff->nodeList[208].symbol = 207;
  huff->nodeList[413].left = &huff->nodeList[338];
  huff->nodeList[209].parent = &huff->nodeList[338];
  huff->nodeList[209].weight = 6020;
  huff->nodeList[209].symbol = 208;
  huff->nodeList[210].parent = huff->nodeList[356].left;
  huff->nodeList[210].weight = 3464;
  huff->nodeList[210].symbol = 209;
  huff->nodeList[382].right = &huff->nodeList[301];
  huff->nodeList[211].parent = &huff->nodeList[301];
  huff->nodeList[211].weight = 4431;
  huff->nodeList[211].symbol = 210;
  huff->nodeList[212].parent = huff->nodeList[357].left;
  huff->nodeList[212].weight = 3495;
  huff->nodeList[212].symbol = 211;
  huff->nodeList[213].parent = huff->nodeList[335].right;
  huff->nodeList[213].weight = 2906;
  huff->nodeList[213].symbol = 212;
  huff->nodeList[214].parent = huff->nodeList[258].left;
  huff->nodeList[214].weight = 2243;
  huff->nodeList[214].symbol = 213;
  huff->nodeList[368].right = &huff->nodeList[286];
  huff->nodeList[215].parent = &huff->nodeList[286];
  huff->nodeList[215].weight = 3856;
  huff->nodeList[215].symbol = 214;
  huff->nodeList[216].parent = huff->nodeList[352].left;
  huff->nodeList[216].weight = 3321;
  huff->nodeList[216].symbol = 215;
  huff->nodeList[441].right = &huff->nodeList[381];
  huff->nodeList[217].parent = &huff->nodeList[381];
  huff->nodeList[217].weight = 8759;
  huff->nodeList[217].symbol = 216;
  huff->nodeList[218].parent = huff->nodeList[371].left;
  huff->nodeList[218].weight = 3928;
  huff->nodeList[218].symbol = 217;
  huff->nodeList[219].parent = huff->nodeList[332].right;
  huff->nodeList[219].weight = 2905;
  huff->nodeList[219].symbol = 218;
  huff->nodeList[220].parent = huff->nodeList[370].left;
  huff->nodeList[220].weight = 3875;
  huff->nodeList[220].symbol = 219;
  huff->nodeList[221].parent = huff->nodeList[381].left;
  huff->nodeList[221].weight = 4382;
  huff->nodeList[221].symbol = 220;
  huff->nodeList[222].parent = huff->nodeList[370].right;
  huff->nodeList[222].weight = 3885;
  huff->nodeList[222].symbol = 221;
  huff->nodeList[223].parent = huff->nodeList[409].right;
  huff->nodeList[223].weight = 5869;
  huff->nodeList[223].symbol = 222;
  huff->nodeList[417].left = &huff->nodeList[345];
  huff->nodeList[224].parent = &huff->nodeList[345];
  huff->nodeList[224].weight = 6235;
  huff->nodeList[224].symbol = 223;
  huff->nodeList[225].parent = huff->nodeList[453].left;
  huff->nodeList[225].weight = 10685;
  huff->nodeList[225].symbol = 224;
  huff->nodeList[226].parent = huff->nodeList[382].right;
  huff->nodeList[226].weight = 4433;
  huff->nodeList[226].symbol = 225;
  huff->nodeList[227].parent = huff->nodeList[387].right;
  huff->nodeList[227].weight = 4639;
  huff->nodeList[227].symbol = 226;
  huff->nodeList[228].parent = huff->nodeList[379].right;
  huff->nodeList[228].weight = 4305;
  huff->nodeList[228].symbol = 227;
  huff->nodeList[390].left = &huff->nodeList[310];
  huff->nodeList[229].parent = &huff->nodeList[310];
  huff->nodeList[229].weight = 4683;
  huff->nodeList[229].symbol = 228;
  huff->nodeList[230].parent = huff->nodeList[332].left;
  huff->nodeList[230].weight = 2849;
  huff->nodeList[230].symbol = 229;
  huff->nodeList[231].parent = huff->nodeList[353].left;
  huff->nodeList[231].weight = 3379;
  huff->nodeList[231].symbol = 230;
  huff->nodeList[232].parent = &huff->nodeList[310];
  huff->nodeList[232].weight = 4683;
  huff->nodeList[232].symbol = 231;
  huff->nodeList[233].parent = huff->nodeList[404].left;
  huff->nodeList[233].weight = 5477;
  huff->nodeList[233].symbol = 232;
  huff->nodeList[234].parent = huff->nodeList[377].left;
  huff->nodeList[234].weight = 4127;
  huff->nodeList[234].symbol = 233;
  huff->nodeList[235].parent = huff->nodeList[368].right;
  huff->nodeList[235].weight = 3853;
  huff->nodeList[235].symbol = 234;
  huff->nodeList[236].parent = huff->nodeList[358].right;
  huff->nodeList[236].weight = 3515;
  huff->nodeList[236].symbol = 235;
  huff->nodeList[237].parent = huff->nodeList[395].right;
  huff->nodeList[237].weight = 4913;
  huff->nodeList[237].symbol = 236;
  huff->nodeList[238].parent = huff->nodeList[360].right;
  huff->nodeList[238].weight = 3601;
  huff->nodeList[238].symbol = 237;
  huff->nodeList[239].parent = huff->nodeList[401].left;
  huff->nodeList[239].weight = 5237;
  huff->nodeList[239].symbol = 238;
  huff->nodeList[421].left = &huff->nodeList[351];
  huff->nodeList[240].parent = &huff->nodeList[351];
  huff->nodeList[240].weight = 6617;
  huff->nodeList[240].symbol = 239;
  huff->nodeList[443].right = &huff->nodeList[384];
  huff->nodeList[241].parent = &huff->nodeList[384];
  huff->nodeList[241].weight = 9019;
  huff->nodeList[241].symbol = 240;
  huff->nodeList[242].parent = huff->nodeList[395].left;
  huff->nodeList[242].weight = 4857;
  huff->nodeList[242].symbol = 241;
  huff->nodeList[243].parent = huff->nodeList[375].right;
  huff->nodeList[243].weight = 4112;
  huff->nodeList[243].symbol = 242;
  huff->nodeList[244].parent = huff->nodeList[399].right;
  huff->nodeList[244].weight = 5180;
  huff->nodeList[244].symbol = 243;
  huff->nodeList[245].parent = huff->nodeList[412].right;
  huff->nodeList[245].weight = 5998;
  huff->nodeList[245].symbol = 244;
  huff->nodeList[246].parent = huff->nodeList[396].left;
  huff->nodeList[246].weight = 4925;
  huff->nodeList[246].symbol = 245;
  huff->nodeList[247].parent = huff->nodeList[397].left;
  huff->nodeList[247].weight = 4986;
  huff->nodeList[247].symbol = 246;
  huff->nodeList[248].parent = huff->nodeList[418].right;
  huff->nodeList[248].weight = 6365;
  huff->nodeList[248].symbol = 247;
  huff->nodeList[436].right = &huff->nodeList[372];
  huff->nodeList[249].parent = &huff->nodeList[372];
  huff->nodeList[249].weight = 7930;
  huff->nodeList[249].symbol = 248;
  huff->nodeList[250].parent = huff->nodeList[412].left;
  huff->nodeList[250].weight = 5948;
  huff->nodeList[250].symbol = 249;
  huff->nodeList[437].right = &huff->nodeList[374];
  huff->nodeList[251].parent = &huff->nodeList[374];
  huff->nodeList[251].weight = 8085;
  huff->nodeList[251].symbol = 250;
  huff->nodeList[252].parent = huff->nodeList[435].left;
  huff->nodeList[252].weight = 7732;
  huff->nodeList[252].symbol = 251;
  huff->nodeList[440].right = &huff->nodeList[379];
  huff->nodeList[253].parent = &huff->nodeList[379];
  huff->nodeList[253].weight = 8643;
  huff->nodeList[253].symbol = 252;
  huff->nodeList[254].parent = huff->nodeList[443].left;
  huff->nodeList[254].weight = 8901;
  huff->nodeList[254].symbol = 253;
  huff->nodeList[448].left = &huff->nodeList[393];
  huff->nodeList[255].parent = &huff->nodeList[393];
  huff->nodeList[255].weight = 9653;
  huff->nodeList[255].symbol = 254;
  huff->nodeList[492].left = &huff->nodeList[472];
  huff->nodeList[256].parent = &huff->nodeList[472];
  huff->nodeList[256].weight = 32647;
  huff->nodeList[256].symbol = 255;
  huff->nodeList[257].parent = &huff->nodeList[258];
  huff->nodeList[257].weight = 2243;
  huff->nodeList[257].symbol = 257;
  huff->nodeList[258].parent = huff->nodeList[389].right;
  huff->nodeList[258].weight = 4679;
  huff->nodeList[258].symbol = 257;
  huff->nodeList[259].parent = huff->nodeList[402].left;
  huff->nodeList[259].weight = 5284;
  huff->nodeList[259].symbol = 257;
  huff->nodeList[260].parent = huff->nodeList[406].left;
  huff->nodeList[260].weight = 5563;
  huff->nodeList[260].symbol = 257;
  huff->nodeList[408].left = &huff->nodeList[332];
  huff->nodeList[261].parent = &huff->nodeList[332];
  huff->nodeList[261].weight = 5728;
  huff->nodeList[261].symbol = 257;
  huff->nodeList[262].parent = &huff->nodeList[332];
  huff->nodeList[262].weight = 5791;
  huff->nodeList[262].symbol = 257;
  huff->nodeList[263].parent = huff->nodeList[410].left;
  huff->nodeList[263].weight = 5898;
  huff->nodeList[263].symbol = 257;
  huff->nodeList[264].parent = huff->nodeList[413].left;
  huff->nodeList[264].weight = 6013;
  huff->nodeList[264].symbol = 257;
  huff->nodeList[265].parent = huff->nodeList[415].left;
  huff->nodeList[265].weight = 6147;
  huff->nodeList[265].symbol = 257;
  huff->nodeList[266].parent = huff->nodeList[416].right;
  huff->nodeList[266].weight = 6209;
  huff->nodeList[266].symbol = 257;
  huff->nodeList[267].parent = huff->nodeList[417].left;
  huff->nodeList[267].weight = 6270;
  huff->nodeList[267].symbol = 257;
  huff->nodeList[268].parent = huff->nodeList[418].left;
  huff->nodeList[268].weight = 6339;
  huff->nodeList[268].symbol = 257;
  huff->nodeList[269].parent = huff->nodeList[420].left;
  huff->nodeList[269].weight = 6450;
  huff->nodeList[269].symbol = 257;
  huff->nodeList[270].parent = huff->nodeList[421].left;
  huff->nodeList[270].weight = 6558;
  huff->nodeList[270].symbol = 257;
  huff->nodeList[271].parent = huff->nodeList[422].left;
  huff->nodeList[271].weight = 6643;
  huff->nodeList[271].symbol = 257;
  huff->nodeList[272].parent = huff->nodeList[422].right;
  huff->nodeList[272].weight = 6764;
  huff->nodeList[272].symbol = 257;
  huff->nodeList[423].left = &huff->nodeList[354];
  huff->nodeList[273].parent = &huff->nodeList[354];
  huff->nodeList[273].weight = 6803;
  huff->nodeList[273].symbol = 257;
  huff->nodeList[274].parent = &huff->nodeList[354];
  huff->nodeList[274].weight = 6824;
  huff->nodeList[274].symbol = 257;
  huff->nodeList[425].left = &huff->nodeList[355];
  huff->nodeList[275].parent = &huff->nodeList[355];
  huff->nodeList[275].weight = 6858;
  huff->nodeList[275].symbol = 257;
  huff->nodeList[276].parent = &huff->nodeList[355];
  huff->nodeList[276].weight = 6899;
  huff->nodeList[276].symbol = 257;
  huff->nodeList[277].parent = huff->nodeList[425].right;
  huff->nodeList[277].weight = 6939;
  huff->nodeList[277].symbol = 257;
  huff->nodeList[278].parent = huff->nodeList[426].left;
  huff->nodeList[278].weight = 6974;
  huff->nodeList[278].symbol = 257;
  huff->nodeList[426].right = &huff->nodeList[358];
  huff->nodeList[279].parent = &huff->nodeList[358];
  huff->nodeList[279].weight = 7015;
  huff->nodeList[279].symbol = 257;
  huff->nodeList[280].parent = &huff->nodeList[358];
  huff->nodeList[280].weight = 7048;
  huff->nodeList[280].symbol = 257;
  huff->nodeList[281].parent = huff->nodeList[428].left;
  huff->nodeList[281].weight = 7098;
  huff->nodeList[281].symbol = 257;
  huff->nodeList[282].parent = huff->nodeList[428].right;
  huff->nodeList[282].weight = 7164;
  huff->nodeList[282].symbol = 257;
  huff->nodeList[283].parent = huff->nodeList[431].left;
  huff->nodeList[283].weight = 7377;
  huff->nodeList[283].symbol = 257;
  huff->nodeList[284].parent = huff->nodeList[431].right;
  huff->nodeList[284].weight = 7424;
  huff->nodeList[284].symbol = 257;
  huff->nodeList[285].parent = huff->nodeList[432].left;
  huff->nodeList[285].weight = 7574;
  huff->nodeList[285].symbol = 257;
  huff->nodeList[286].parent = huff->nodeList[434].left;
  huff->nodeList[286].weight = 7709;
  huff->nodeList[286].symbol = 257;
  huff->nodeList[435].right = &huff->nodeList[370];
  huff->nodeList[287].parent = &huff->nodeList[370];
  huff->nodeList[287].weight = 7753;
  huff->nodeList[287].symbol = 257;
  huff->nodeList[288].parent = &huff->nodeList[370];
  huff->nodeList[288].weight = 7774;
  huff->nodeList[288].symbol = 257;
  huff->nodeList[289].parent = huff->nodeList[436].left;
  huff->nodeList[289].weight = 7817;
  huff->nodeList[289].symbol = 257;
  huff->nodeList[290].parent = huff->nodeList[436].right;
  huff->nodeList[290].weight = 7985;
  huff->nodeList[290].symbol = 257;
  huff->nodeList[291].parent = huff->nodeList[437].right;
  huff->nodeList[291].weight = 8057;
  huff->nodeList[291].symbol = 257;
  huff->nodeList[438].left = &huff->nodeList[375];
  huff->nodeList[292].parent = &huff->nodeList[375];
  huff->nodeList[292].weight = 8174;
  huff->nodeList[292].symbol = 257;
  huff->nodeList[293].parent = &huff->nodeList[375];
  huff->nodeList[293].weight = 8206;
  huff->nodeList[293].symbol = 257;
  huff->nodeList[439].right = &huff->nodeList[377];
  huff->nodeList[294].parent = &huff->nodeList[377];
  huff->nodeList[294].weight = 8290;
  huff->nodeList[294].symbol = 257;
  huff->nodeList[295].parent = &huff->nodeList[377];
  huff->nodeList[295].weight = 8367;
  huff->nodeList[295].symbol = 257;
  huff->nodeList[440].left = &huff->nodeList[378];
  huff->nodeList[296].parent = &huff->nodeList[378];
  huff->nodeList[296].weight = 8408;
  huff->nodeList[296].symbol = 257;
  huff->nodeList[297].parent = &huff->nodeList[378];
  huff->nodeList[297].weight = 8519;
  huff->nodeList[297].symbol = 257;
  huff->nodeList[298].parent = huff->nodeList[440].right;
  huff->nodeList[298].weight = 8647;
  huff->nodeList[298].symbol = 257;
  huff->nodeList[299].parent = huff->nodeList[441].right;
  huff->nodeList[299].weight = 8727;
  huff->nodeList[299].symbol = 257;
  huff->nodeList[442].left = &huff->nodeList[382];
  huff->nodeList[300].parent = &huff->nodeList[382];
  huff->nodeList[300].weight = 8825;
  huff->nodeList[300].symbol = 257;
  huff->nodeList[301].parent = &huff->nodeList[382];
  huff->nodeList[301].weight = 8864;
  huff->nodeList[301].symbol = 257;
  huff->nodeList[302].parent = huff->nodeList[443].right;
  huff->nodeList[302].weight = 8961;
  huff->nodeList[302].symbol = 257;
  huff->nodeList[444].left = &huff->nodeList[385];
  huff->nodeList[303].parent = &huff->nodeList[385];
  huff->nodeList[303].weight = 9048;
  huff->nodeList[303].symbol = 257;
  huff->nodeList[304].parent = &huff->nodeList[385];
  huff->nodeList[304].weight = 9108;
  huff->nodeList[304].symbol = 257;
  huff->nodeList[305].parent = huff->nodeList[444].right;
  huff->nodeList[305].weight = 9145;
  huff->nodeList[305].symbol = 257;
  huff->nodeList[445].left = &huff->nodeList[387];
  huff->nodeList[306].parent = &huff->nodeList[387];
  huff->nodeList[306].weight = 9177;
  huff->nodeList[306].symbol = 257;
  huff->nodeList[307].parent = &huff->nodeList[387];
  huff->nodeList[307].weight = 9287;
  huff->nodeList[307].symbol = 257;
  huff->nodeList[308].parent = huff->nodeList[445].right;
  huff->nodeList[308].weight = 9307;
  huff->nodeList[308].symbol = 257;
  huff->nodeList[309].right = &huff->nodeList[258];
  huff->nodeList[309].parent = huff->nodeList[446].left;
  huff->nodeList[309].weight = 9356;
  huff->nodeList[309].symbol = 257;
  huff->nodeList[310].parent = huff->nodeList[446].right;
  huff->nodeList[310].weight = 9366;
  huff->nodeList[310].symbol = 257;
  huff->nodeList[311].parent = huff->nodeList[447].left;
  huff->nodeList[311].weight = 9403;
  huff->nodeList[311].symbol = 257;
  huff->nodeList[312].parent = huff->nodeList[447].right;
  huff->nodeList[312].weight = 9485;
  huff->nodeList[312].symbol = 257;
  huff->nodeList[313].parent = huff->nodeList[448].left;
  huff->nodeList[313].weight = 9624;
  huff->nodeList[313].symbol = 257;
  huff->nodeList[449].left = &huff->nodeList[395];
  huff->nodeList[314].parent = &huff->nodeList[395];
  huff->nodeList[314].weight = 9684;
  huff->nodeList[314].symbol = 257;
  huff->nodeList[315].parent = &huff->nodeList[395];
  huff->nodeList[315].weight = 9780;
  huff->nodeList[315].symbol = 257;
  huff->nodeList[449].right = &huff->nodeList[396];
  huff->nodeList[316].parent = &huff->nodeList[396];
  huff->nodeList[316].weight = 9854;
  huff->nodeList[316].symbol = 257;
  huff->nodeList[317].parent = &huff->nodeList[396];
  huff->nodeList[317].weight = 9915;
  huff->nodeList[317].symbol = 257;
  huff->nodeList[450].left = &huff->nodeList[397];
  huff->nodeList[318].parent = &huff->nodeList[397];
  huff->nodeList[318].weight = 9968;
  huff->nodeList[318].symbol = 257;
  huff->nodeList[319].parent = &huff->nodeList[397];
  huff->nodeList[319].weight = 9977;
  huff->nodeList[319].symbol = 257;
  huff->nodeList[320].parent = huff->nodeList[450].right;
  huff->nodeList[320].weight = 10028;
  huff->nodeList[320].symbol = 257;
  huff->nodeList[321].parent = huff->nodeList[451].left;
  huff->nodeList[321].weight = 10344;
  huff->nodeList[321].symbol = 257;
  huff->nodeList[451].right = &huff->nodeList[400];
  huff->nodeList[322].parent = &huff->nodeList[400];
  huff->nodeList[322].weight = 10389;
  huff->nodeList[322].symbol = 257;
  huff->nodeList[323].parent = &huff->nodeList[400];
  huff->nodeList[323].weight = 10427;
  huff->nodeList[323].symbol = 257;
  huff->nodeList[324].parent = huff->nodeList[452].left;
  huff->nodeList[324].weight = 10466;
  huff->nodeList[324].symbol = 257;
  huff->nodeList[452].right = &huff->nodeList[402];
  huff->nodeList[325].parent = &huff->nodeList[402];
  huff->nodeList[325].weight = 10581;
  huff->nodeList[325].symbol = 257;
  huff->nodeList[326].parent = &huff->nodeList[402];
  huff->nodeList[326].weight = 10656;
  huff->nodeList[326].symbol = 257;
  huff->nodeList[453].right = &huff->nodeList[404];
  huff->nodeList[327].parent = &huff->nodeList[404];
  huff->nodeList[327].weight = 10864;
  huff->nodeList[327].symbol = 257;
  huff->nodeList[328].parent = &huff->nodeList[404];
  huff->nodeList[328].weight = 11006;
  huff->nodeList[328].symbol = 257;
  huff->nodeList[454].right = &huff->nodeList[406];
  huff->nodeList[329].parent = &huff->nodeList[406];
  huff->nodeList[329].weight = 11146;
  huff->nodeList[329].symbol = 257;
  huff->nodeList[330].parent = &huff->nodeList[406];
  huff->nodeList[330].weight = 11296;
  huff->nodeList[330].symbol = 257;
  huff->nodeList[331].parent = huff->nodeList[455].left;
  huff->nodeList[331].weight = 11377;
  huff->nodeList[331].symbol = 257;
  huff->nodeList[332].parent = huff->nodeList[455].right;
  huff->nodeList[332].weight = 11519;
  huff->nodeList[332].symbol = 257;
  huff->nodeList[456].left = &huff->nodeList[409];
  huff->nodeList[333].parent = &huff->nodeList[409];
  huff->nodeList[333].weight = 11658;
  huff->nodeList[333].symbol = 257;
  huff->nodeList[334].parent = &huff->nodeList[409];
  huff->nodeList[334].weight = 11735;
  huff->nodeList[334].symbol = 257;
  huff->nodeList[335].parent = huff->nodeList[456].right;
  huff->nodeList[335].weight = 11779;
  huff->nodeList[335].symbol = 257;
  huff->nodeList[457].right = &huff->nodeList[412];
  huff->nodeList[336].parent = &huff->nodeList[412];
  huff->nodeList[336].weight = 11916;
  huff->nodeList[336].symbol = 257;
  huff->nodeList[337].parent = &huff->nodeList[412];
  huff->nodeList[337].weight = 11970;
  huff->nodeList[337].symbol = 257;
  huff->nodeList[458].left = &huff->nodeList[413];
  huff->nodeList[338].parent = &huff->nodeList[413];
  huff->nodeList[338].weight = 12033;
  huff->nodeList[338].symbol = 257;
  huff->nodeList[339].parent = &huff->nodeList[413];
  huff->nodeList[339].weight = 12092;
  huff->nodeList[339].symbol = 257;
  huff->nodeList[458].right = &huff->nodeList[414];
  huff->nodeList[340].parent = &huff->nodeList[414];
  huff->nodeList[340].weight = 12153;
  huff->nodeList[340].symbol = 257;
  huff->nodeList[341].parent = &huff->nodeList[414];
  huff->nodeList[341].weight = 12195;
  huff->nodeList[341].symbol = 257;
  huff->nodeList[459].left = &huff->nodeList[415];
  huff->nodeList[342].parent = &huff->nodeList[415];
  huff->nodeList[342].weight = 12250;
  huff->nodeList[342].symbol = 257;
  huff->nodeList[343].parent = &huff->nodeList[415];
  huff->nodeList[343].weight = 12351;
  huff->nodeList[343].symbol = 257;
  huff->nodeList[344].parent = huff->nodeList[459].right;
  huff->nodeList[344].weight = 12408;
  huff->nodeList[344].symbol = 257;
  huff->nodeList[345].parent = huff->nodeList[460].left;
  huff->nodeList[345].weight = 12505;
  huff->nodeList[345].symbol = 257;
  huff->nodeList[460].right = &huff->nodeList[418];
  huff->nodeList[346].parent = &huff->nodeList[418];
  huff->nodeList[346].weight = 12672;
  huff->nodeList[346].symbol = 257;
  huff->nodeList[347].parent = &huff->nodeList[418];
  huff->nodeList[347].weight = 12714;
  huff->nodeList[347].symbol = 257;
  huff->nodeList[348].parent = huff->nodeList[461].left;
  huff->nodeList[348].weight = 12857;
  huff->nodeList[348].symbol = 257;
  huff->nodeList[461].right = &huff->nodeList[420];
  huff->nodeList[349].parent = &huff->nodeList[420];
  huff->nodeList[349].weight = 12896;
  huff->nodeList[349].symbol = 257;
  huff->nodeList[350].parent = &huff->nodeList[420];
  huff->nodeList[350].weight = 13041;
  huff->nodeList[350].symbol = 257;
  huff->nodeList[351].parent = huff->nodeList[462].left;
  huff->nodeList[351].weight = 13175;
  huff->nodeList[351].symbol = 257;
  huff->nodeList[463].left = &huff->nodeList[422];
  huff->nodeList[352].parent = &huff->nodeList[422];
  huff->nodeList[352].weight = 13376;
  huff->nodeList[352].symbol = 257;
  huff->nodeList[353].parent = &huff->nodeList[422];
  huff->nodeList[353].weight = 13543;
  huff->nodeList[353].symbol = 257;
  huff->nodeList[354].parent = huff->nodeList[463].right;
  huff->nodeList[354].weight = 13627;
  huff->nodeList[354].symbol = 257;
  huff->nodeList[464].right = &huff->nodeList[425];
  huff->nodeList[355].parent = &huff->nodeList[425];
  huff->nodeList[355].weight = 13757;
  huff->nodeList[355].symbol = 257;
  huff->nodeList[356].parent = &huff->nodeList[425];
  huff->nodeList[356].weight = 13896;
  huff->nodeList[356].symbol = 257;
  huff->nodeList[465].right = &huff->nodeList[426];
  huff->nodeList[357].parent = &huff->nodeList[426];
  huff->nodeList[357].weight = 13953;
  huff->nodeList[357].symbol = 257;
  huff->nodeList[358].parent = &huff->nodeList[426];
  huff->nodeList[358].weight = 14063;
  huff->nodeList[358].symbol = 257;
  huff->nodeList[466].right = &huff->nodeList[428];
  huff->nodeList[359].parent = &huff->nodeList[428];
  huff->nodeList[359].weight = 14154;
  huff->nodeList[359].symbol = 257;
  huff->nodeList[360].parent = &huff->nodeList[428];
  huff->nodeList[360].weight = 14276;
  huff->nodeList[360].symbol = 257;
  huff->nodeList[361].parent = huff->nodeList[467].left;
  huff->nodeList[361].weight = 14408;
  huff->nodeList[361].symbol = 257;
  huff->nodeList[467].right = &huff->nodeList[430];
  huff->nodeList[362].parent = &huff->nodeList[430];
  huff->nodeList[362].weight = 14433;
  huff->nodeList[362].symbol = 257;
  huff->nodeList[363].parent = &huff->nodeList[430];
  huff->nodeList[363].weight = 14484;
  huff->nodeList[363].symbol = 257;
  huff->nodeList[468].right = &huff->nodeList[431];
  huff->nodeList[364].parent = &huff->nodeList[431];
  huff->nodeList[364].weight = 14699;
  huff->nodeList[364].symbol = 257;
  huff->nodeList[365].parent = &huff->nodeList[431];
  huff->nodeList[365].weight = 14933;
  huff->nodeList[365].symbol = 257;
  huff->nodeList[366].parent = huff->nodeList[469].left;
  huff->nodeList[366].weight = 15142;
  huff->nodeList[366].symbol = 257;
  huff->nodeList[367].parent = huff->nodeList[469].right;
  huff->nodeList[367].weight = 15323;
  huff->nodeList[367].symbol = 257;
  huff->nodeList[368].parent = huff->nodeList[470].left;
  huff->nodeList[368].weight = 15407;
  huff->nodeList[368].symbol = 257;
  huff->nodeList[470].right = &huff->nodeList[435];
  huff->nodeList[369].parent = &huff->nodeList[435];
  huff->nodeList[369].weight = 15482;
  huff->nodeList[369].symbol = 257;
  huff->nodeList[370].parent = &huff->nodeList[435];
  huff->nodeList[370].weight = 15527;
  huff->nodeList[370].symbol = 257;
  huff->nodeList[471].left = &huff->nodeList[436];
  huff->nodeList[371].parent = &huff->nodeList[436];
  huff->nodeList[371].weight = 15643;
  huff->nodeList[371].symbol = 257;
  huff->nodeList[372].parent = &huff->nodeList[436];
  huff->nodeList[372].weight = 15915;
  huff->nodeList[372].symbol = 257;
  huff->nodeList[471].right = &huff->nodeList[437];
  huff->nodeList[373].parent = &huff->nodeList[437];
  huff->nodeList[373].weight = 16022;
  huff->nodeList[373].symbol = 257;
  huff->nodeList[374].parent = &huff->nodeList[437];
  huff->nodeList[374].weight = 16142;
  huff->nodeList[374].symbol = 257;
  huff->nodeList[375].parent = huff->nodeList[472].right;
  huff->nodeList[375].weight = 16380;
  huff->nodeList[375].symbol = 257;
  huff->nodeList[473].left = &huff->nodeList[439];
  huff->nodeList[376].parent = &huff->nodeList[439];
  huff->nodeList[376].weight = 16530;
  huff->nodeList[376].symbol = 257;
  huff->nodeList[377].parent = &huff->nodeList[439];
  huff->nodeList[377].weight = 16657;
  huff->nodeList[377].symbol = 257;
  huff->nodeList[473].right = &huff->nodeList[440];
  huff->nodeList[378].parent = &huff->nodeList[440];
  huff->nodeList[378].weight = 16927;
  huff->nodeList[378].symbol = 257;
  huff->nodeList[379].parent = &huff->nodeList[440];
  huff->nodeList[379].weight = 17290;
  huff->nodeList[379].symbol = 257;
  huff->nodeList[474].left = &huff->nodeList[441];
  huff->nodeList[380].parent = &huff->nodeList[441];
  huff->nodeList[380].weight = 17342;
  huff->nodeList[380].symbol = 257;
  huff->nodeList[381].parent = &huff->nodeList[441];
  huff->nodeList[381].weight = 17486;
  huff->nodeList[381].symbol = 257;
  huff->nodeList[382].parent = huff->nodeList[474].right;
  huff->nodeList[382].weight = 17689;
  huff->nodeList[382].symbol = 257;
  huff->nodeList[475].left = &huff->nodeList[443];
  huff->nodeList[383].parent = &huff->nodeList[443];
  huff->nodeList[383].weight = 17775;
  huff->nodeList[383].symbol = 257;
  huff->nodeList[384].parent = &huff->nodeList[443];
  huff->nodeList[384].weight = 17980;
  huff->nodeList[384].symbol = 257;
  huff->nodeList[475].right = &huff->nodeList[444];
  huff->nodeList[385].parent = &huff->nodeList[444];
  huff->nodeList[385].weight = 18156;
  huff->nodeList[385].symbol = 257;
  huff->nodeList[386].parent = &huff->nodeList[444];
  huff->nodeList[386].weight = 18315;
  huff->nodeList[386].symbol = 257;
  huff->nodeList[476].left = &huff->nodeList[445];
  huff->nodeList[387].parent = &huff->nodeList[445];
  huff->nodeList[387].weight = 18464;
  huff->nodeList[387].symbol = 257;
  huff->nodeList[388].parent = &huff->nodeList[445];
  huff->nodeList[388].weight = 18653;
  huff->nodeList[388].symbol = 257;
  huff->nodeList[476].right = &huff->nodeList[446];
  huff->nodeList[389].parent = &huff->nodeList[446];
  huff->nodeList[389].weight = 18709;
  huff->nodeList[389].symbol = 257;
  huff->nodeList[390].parent = &huff->nodeList[446];
  huff->nodeList[390].weight = 18734;
  huff->nodeList[390].symbol = 257;
  huff->nodeList[477].left = &huff->nodeList[447];
  huff->nodeList[391].parent = &huff->nodeList[447];
  huff->nodeList[391].weight = 18772;
  huff->nodeList[391].symbol = 257;
  huff->nodeList[392].parent = &huff->nodeList[447];
  huff->nodeList[392].weight = 18905;
  huff->nodeList[392].symbol = 257;
  huff->nodeList[477].right = &huff->nodeList[448];
  huff->nodeList[393].parent = &huff->nodeList[448];
  huff->nodeList[393].weight = 19277;
  huff->nodeList[393].symbol = 257;
  huff->nodeList[394].parent = &huff->nodeList[448];
  huff->nodeList[394].weight = 19333;
  huff->nodeList[394].symbol = 257;
  huff->nodeList[478].right = &huff->nodeList[449];
  huff->nodeList[395].parent = &huff->nodeList[449];
  huff->nodeList[395].weight = 19464;
  huff->nodeList[395].symbol = 257;
  huff->nodeList[396].parent = &huff->nodeList[449];
  huff->nodeList[396].weight = 19769;
  huff->nodeList[396].symbol = 257;
  huff->nodeList[479].left = &huff->nodeList[450];
  huff->nodeList[397].parent = &huff->nodeList[450];
  huff->nodeList[397].weight = 19945;
  huff->nodeList[397].symbol = 257;
  huff->nodeList[398].parent = &huff->nodeList[450];
  huff->nodeList[398].weight = 20007;
  huff->nodeList[398].symbol = 257;
  huff->nodeList[480].right = &huff->nodeList[451];
  huff->nodeList[399].parent = &huff->nodeList[451];
  huff->nodeList[399].weight = 20605;
  huff->nodeList[399].symbol = 257;
  huff->nodeList[400].parent = &huff->nodeList[451];
  huff->nodeList[400].weight = 20816;
  huff->nodeList[400].symbol = 257;
  huff->nodeList[481].left = &huff->nodeList[452];
  huff->nodeList[401].parent = &huff->nodeList[452];
  huff->nodeList[401].weight = 21000;
  huff->nodeList[401].symbol = 257;
  huff->nodeList[402].parent = &huff->nodeList[452];
  huff->nodeList[402].weight = 21237;
  huff->nodeList[402].symbol = 257;
  huff->nodeList[481].right = &huff->nodeList[453];
  huff->nodeList[403].parent = &huff->nodeList[453];
  huff->nodeList[403].weight = 21443;
  huff->nodeList[403].symbol = 257;
  huff->nodeList[404].parent = &huff->nodeList[453];
  huff->nodeList[404].weight = 21870;
  huff->nodeList[404].symbol = 257;
  huff->nodeList[482].left = &huff->nodeList[454];
  huff->nodeList[405].parent = &huff->nodeList[454];
  huff->nodeList[405].weight = 22213;
  huff->nodeList[405].symbol = 257;
  huff->nodeList[406].parent = &huff->nodeList[454];
  huff->nodeList[406].weight = 22442;
  huff->nodeList[406].symbol = 257;
  huff->nodeList[482].right = &huff->nodeList[455];
  huff->nodeList[407].parent = &huff->nodeList[455];
  huff->nodeList[407].weight = 22797;
  huff->nodeList[407].symbol = 257;
  huff->nodeList[408].parent = &huff->nodeList[455];
  huff->nodeList[408].weight = 23147;
  huff->nodeList[408].symbol = 257;
  huff->nodeList[483].left = &huff->nodeList[456];
  huff->nodeList[409].parent = &huff->nodeList[456];
  huff->nodeList[409].weight = 23393;
  huff->nodeList[409].symbol = 257;
  huff->nodeList[410].parent = &huff->nodeList[456];
  huff->nodeList[410].weight = 23577;
  huff->nodeList[410].symbol = 257;
  huff->nodeList[483].right = &huff->nodeList[457];
  huff->nodeList[411].parent = &huff->nodeList[457];
  huff->nodeList[411].weight = 23709;
  huff->nodeList[411].symbol = 257;
  huff->nodeList[412].parent = &huff->nodeList[457];
  huff->nodeList[412].weight = 23886;
  huff->nodeList[412].symbol = 257;
  huff->nodeList[484].right = &huff->nodeList[458];
  huff->nodeList[413].parent = &huff->nodeList[458];
  huff->nodeList[413].weight = 24125;
  huff->nodeList[413].symbol = 257;
  huff->nodeList[414].parent = &huff->nodeList[458];
  huff->nodeList[414].weight = 24348;
  huff->nodeList[414].symbol = 257;
  huff->nodeList[485].right = &huff->nodeList[459];
  huff->nodeList[415].parent = &huff->nodeList[459];
  huff->nodeList[415].weight = 24601;
  huff->nodeList[415].symbol = 257;
  huff->nodeList[416].parent = &huff->nodeList[459];
  huff->nodeList[416].weight = 24794;
  huff->nodeList[416].symbol = 257;
  huff->nodeList[486].left = &huff->nodeList[460];
  huff->nodeList[417].parent = &huff->nodeList[460];
  huff->nodeList[417].weight = 25111;
  huff->nodeList[417].symbol = 257;
  huff->nodeList[418].parent = &huff->nodeList[460];
  huff->nodeList[418].weight = 25386;
  huff->nodeList[418].symbol = 257;
  huff->nodeList[486].right = &huff->nodeList[461];
  huff->nodeList[419].parent = &huff->nodeList[461];
  huff->nodeList[419].weight = 25671;
  huff->nodeList[419].symbol = 257;
  huff->nodeList[420].parent = &huff->nodeList[461];
  huff->nodeList[420].weight = 25937;
  huff->nodeList[420].symbol = 257;
  huff->nodeList[421].parent = huff->nodeList[487].left;
  huff->nodeList[421].weight = 26359;
  huff->nodeList[421].symbol = 257;
  huff->nodeList[487].right = &huff->nodeList[463];
  huff->nodeList[422].parent = &huff->nodeList[463];
  huff->nodeList[422].weight = 26919;
  huff->nodeList[422].symbol = 257;
  huff->nodeList[423].parent = &huff->nodeList[463];
  huff->nodeList[423].weight = 27256;
  huff->nodeList[423].symbol = 257;
  huff->nodeList[488].left = &huff->nodeList[464];
  huff->nodeList[424].parent = &huff->nodeList[464];
  huff->nodeList[424].weight = 27323;
  huff->nodeList[424].symbol = 257;
  huff->nodeList[425].parent = &huff->nodeList[464];
  huff->nodeList[425].weight = 27653;
  huff->nodeList[425].symbol = 257;
  huff->nodeList[426].parent = huff->nodeList[488].right;
  huff->nodeList[426].weight = 28016;
  huff->nodeList[426].symbol = 257;
  huff->nodeList[489].left = &huff->nodeList[466];
  huff->nodeList[427].parent = &huff->nodeList[466];
  huff->nodeList[427].weight = 28225;
  huff->nodeList[427].symbol = 257;
  huff->nodeList[428].parent = &huff->nodeList[466];
  huff->nodeList[428].weight = 28430;
  huff->nodeList[428].symbol = 257;
  huff->nodeList[489].right = &huff->nodeList[467];
  huff->nodeList[429].parent = &huff->nodeList[467];
  huff->nodeList[429].weight = 28727;
  huff->nodeList[429].symbol = 257;
  huff->nodeList[430].parent = &huff->nodeList[467];
  huff->nodeList[430].weight = 28917;
  huff->nodeList[430].symbol = 257;
  huff->nodeList[431].parent = huff->nodeList[490].left;
  huff->nodeList[431].weight = 29632;
  huff->nodeList[431].symbol = 257;
  huff->nodeList[490].right = &huff->nodeList[469];
  huff->nodeList[432].parent = &huff->nodeList[469];
  huff->nodeList[432].weight = 30398;
  huff->nodeList[432].symbol = 257;
  huff->nodeList[433].parent = &huff->nodeList[469];
  huff->nodeList[433].weight = 30704;
  huff->nodeList[433].symbol = 257;
  huff->nodeList[491].left = &huff->nodeList[470];
  huff->nodeList[434].parent = &huff->nodeList[470];
  huff->nodeList[434].weight = 30846;
  huff->nodeList[434].symbol = 257;
  huff->nodeList[435].parent = &huff->nodeList[470];
  huff->nodeList[435].weight = 31009;
  huff->nodeList[435].symbol = 257;
  huff->nodeList[491].right = &huff->nodeList[471];
  huff->nodeList[436].parent = &huff->nodeList[471];
  huff->nodeList[436].weight = 31558;
  huff->nodeList[436].symbol = 257;
  huff->nodeList[437].parent = &huff->nodeList[471];
  huff->nodeList[437].weight = 32164;
  huff->nodeList[437].symbol = 257;
  huff->nodeList[438].parent = huff->nodeList[492].left;
  huff->nodeList[438].weight = 32820;
  huff->nodeList[438].symbol = 257;
  huff->nodeList[492].right = &huff->nodeList[473];
  huff->nodeList[439].parent = &huff->nodeList[473];
  huff->nodeList[439].weight = 33187;
  huff->nodeList[439].symbol = 257;
  huff->nodeList[440].parent = &huff->nodeList[473];
  huff->nodeList[440].weight = 34217;
  huff->nodeList[440].symbol = 257;
  huff->nodeList[441].parent = &huff->nodeList[474];
  huff->nodeList[441].weight = 34828;
  huff->nodeList[441].symbol = 257;
  huff->nodeList[442].parent = &huff->nodeList[474];
  huff->nodeList[442].weight = 35401;
  huff->nodeList[442].symbol = 257;
  huff->nodeList[494].left = &huff->nodeList[475];
  huff->nodeList[443].parent = &huff->nodeList[475];
  huff->nodeList[443].weight = 35755;
  huff->nodeList[443].symbol = 257;
  huff->nodeList[444].parent = &huff->nodeList[475];
  huff->nodeList[444].weight = 36471;
  huff->nodeList[444].symbol = 257;
  huff->nodeList[494].right = &huff->nodeList[476];
  huff->nodeList[445].parent = &huff->nodeList[476];
  huff->nodeList[445].weight = 37117;
  huff->nodeList[445].symbol = 257;
  huff->nodeList[446].parent = &huff->nodeList[476];
  huff->nodeList[446].weight = 37443;
  huff->nodeList[446].symbol = 257;
  huff->nodeList[495].left = &huff->nodeList[477];
  huff->nodeList[447].parent = &huff->nodeList[477];
  huff->nodeList[447].weight = 37677;
  huff->nodeList[447].symbol = 257;
  huff->nodeList[448].parent = &huff->nodeList[477];
  huff->nodeList[448].weight = 38610;
  huff->nodeList[448].symbol = 257;
  huff->nodeList[449].parent = huff->nodeList[495].right;
  huff->nodeList[449].weight = 39233;
  huff->nodeList[449].symbol = 257;
  huff->nodeList[450].parent = huff->nodeList[496].left;
  huff->nodeList[450].weight = 39952;
  huff->nodeList[450].symbol = 257;
  huff->nodeList[451].parent = huff->nodeList[496].right;
  huff->nodeList[451].weight = 41421;
  huff->nodeList[451].symbol = 257;
  huff->nodeList[497].left = &huff->nodeList[481];
  huff->nodeList[452].parent = &huff->nodeList[481];
  huff->nodeList[452].weight = 42237;
  huff->nodeList[452].symbol = 257;
  huff->nodeList[453].parent = &huff->nodeList[481];
  huff->nodeList[453].weight = 43313;
  huff->nodeList[453].symbol = 257;
  huff->nodeList[497].right = &huff->nodeList[482];
  huff->nodeList[454].parent = &huff->nodeList[482];
  huff->nodeList[454].weight = 44655;
  huff->nodeList[454].symbol = 257;
  huff->nodeList[455].parent = &huff->nodeList[482];
  huff->nodeList[455].weight = 45944;
  huff->nodeList[455].symbol = 257;
  huff->nodeList[498].left = &huff->nodeList[483];
  huff->nodeList[456].parent = &huff->nodeList[483];
  huff->nodeList[456].weight = 46970;
  huff->nodeList[456].symbol = 257;
  huff->nodeList[457].parent = &huff->nodeList[483];
  huff->nodeList[457].weight = 47595;
  huff->nodeList[457].symbol = 257;
  huff->nodeList[458].parent = huff->nodeList[498].right;
  huff->nodeList[458].weight = 48473;
  huff->nodeList[458].symbol = 257;
  huff->nodeList[459].parent = huff->nodeList[499].left;
  huff->nodeList[459].weight = 49395;
  huff->nodeList[459].symbol = 257;
  huff->nodeList[499].right = &huff->nodeList[486];
  huff->nodeList[460].parent = &huff->nodeList[486];
  huff->nodeList[460].weight = 50497;
  huff->nodeList[460].symbol = 257;
  huff->nodeList[461].parent = &huff->nodeList[486];
  huff->nodeList[461].weight = 51608;
  huff->nodeList[461].symbol = 257;
  huff->nodeList[500].left = &huff->nodeList[487];
  huff->nodeList[462].parent = &huff->nodeList[487];
  huff->nodeList[462].weight = 53071;
  huff->nodeList[462].symbol = 257;
  huff->nodeList[463].parent = &huff->nodeList[487];
  huff->nodeList[463].weight = 54175;
  huff->nodeList[463].symbol = 257;
  huff->nodeList[500].right = &huff->nodeList[488];
  huff->nodeList[464].parent = &huff->nodeList[488];
  huff->nodeList[464].weight = 54976;
  huff->nodeList[464].symbol = 257;
  huff->nodeList[465].parent = &huff->nodeList[488];
  huff->nodeList[465].weight = 55708;
  huff->nodeList[465].symbol = 257;
  huff->nodeList[501].left = &huff->nodeList[489];
  huff->nodeList[466].parent = &huff->nodeList[489];
  huff->nodeList[466].weight = 56655;
  huff->nodeList[466].symbol = 257;
  huff->nodeList[467].parent = &huff->nodeList[489];
  huff->nodeList[467].weight = 57644;
  huff->nodeList[467].symbol = 257;
  huff->nodeList[501].right = &huff->nodeList[490];
  huff->nodeList[468].parent = &huff->nodeList[490];
  huff->nodeList[468].weight = 58600;
  huff->nodeList[468].symbol = 257;
  huff->nodeList[469].parent = &huff->nodeList[490];
  huff->nodeList[469].weight = 61102;
  huff->nodeList[469].symbol = 257;
  huff->nodeList[502].left = &huff->nodeList[491];
  huff->nodeList[470].parent = &huff->nodeList[491];
  huff->nodeList[470].weight = 61855;
  huff->nodeList[470].symbol = 257;
  huff->nodeList[471].parent = &huff->nodeList[491];
  huff->nodeList[471].weight = 63722;
  huff->nodeList[471].symbol = 257;
  huff->nodeList[502].right = &huff->nodeList[492];
  huff->nodeList[472].parent = &huff->nodeList[492];
  huff->nodeList[472].weight = 65467;
  huff->nodeList[472].symbol = 257;
  huff->nodeList[473].parent = &huff->nodeList[492];
  huff->nodeList[473].weight = 67404;
  huff->nodeList[473].symbol = 257;
  huff->nodeList[474].parent = huff->nodeList[503].left;
  huff->nodeList[474].weight = 70229;
  huff->nodeList[474].symbol = 257;
  huff->nodeList[503].right = &huff->nodeList[494];
  huff->nodeList[475].parent = &huff->nodeList[494];
  huff->nodeList[475].weight = 72226;
  huff->nodeList[475].symbol = 257;
  huff->nodeList[476].parent = &huff->nodeList[494];
  huff->nodeList[476].weight = 74560;
  huff->nodeList[476].symbol = 257;
  huff->nodeList[504].left = &huff->nodeList[495];
  huff->nodeList[477].parent = &huff->nodeList[495];
  huff->nodeList[477].weight = 76287;
  huff->nodeList[477].symbol = 257;
  huff->nodeList[478].parent = &huff->nodeList[495];
  huff->nodeList[478].weight = 78239;
  huff->nodeList[478].symbol = 257;
  huff->nodeList[504].right = &huff->nodeList[496];
  huff->nodeList[479].parent = &huff->nodeList[496];
  huff->nodeList[479].weight = 80218;
  huff->nodeList[479].symbol = 257;
  huff->nodeList[480].parent = &huff->nodeList[496];
  huff->nodeList[480].weight = 81881;
  huff->nodeList[480].symbol = 257;
  huff->nodeList[505].left = &huff->nodeList[497];
  huff->nodeList[481].parent = &huff->nodeList[497];
  huff->nodeList[481].weight = 85550;
  huff->nodeList[481].symbol = 257;
  huff->nodeList[482].parent = &huff->nodeList[497];
  huff->nodeList[482].weight = 90599;
  huff->nodeList[482].symbol = 257;
  huff->nodeList[505].right = &huff->nodeList[498];
  huff->nodeList[483].parent = &huff->nodeList[498];
  huff->nodeList[483].weight = 94565;
  huff->nodeList[483].symbol = 257;
  huff->nodeList[484].parent = &huff->nodeList[498];
  huff->nodeList[484].weight = 96532;
  huff->nodeList[484].symbol = 257;
  huff->nodeList[506].left = &huff->nodeList[499];
  huff->nodeList[485].parent = &huff->nodeList[499];
  huff->nodeList[485].weight = 98025;
  huff->nodeList[485].symbol = 257;
  huff->nodeList[486].parent = &huff->nodeList[499];
  huff->nodeList[486].weight = 102105;
  huff->nodeList[486].symbol = 257;
  huff->nodeList[506].right = &huff->nodeList[500];
  huff->nodeList[487].parent = &huff->nodeList[500];
  huff->nodeList[487].weight = 107246;
  huff->nodeList[487].symbol = 257;
  huff->nodeList[488].parent = &huff->nodeList[500];
  huff->nodeList[488].weight = 110684;
  huff->nodeList[488].symbol = 257;
  huff->nodeList[489].parent = &huff->nodeList[501];
  huff->nodeList[489].weight = 114299;
  huff->nodeList[489].symbol = 257;
  huff->nodeList[490].parent = &huff->nodeList[501];
  huff->nodeList[490].weight = 119702;
  huff->nodeList[490].symbol = 257;
  huff->nodeList[491].parent = &huff->nodeList[502];
  huff->nodeList[491].weight = 125577;
  huff->nodeList[491].symbol = 257;
  huff->nodeList[492].parent = &huff->nodeList[502];
  huff->nodeList[492].weight = 132871;
  huff->nodeList[492].symbol = 257;
  huff->nodeList[493].right = &huff->nodeList[474];
  huff->nodeList[508].right = &huff->nodeList[503];
  huff->nodeList[493].parent = &huff->nodeList[503];
  huff->nodeList[493].weight = 139006;
  huff->nodeList[493].symbol = 257;
  huff->nodeList[494].parent = &huff->nodeList[503];
  huff->nodeList[494].weight = 146786;
  huff->nodeList[494].symbol = 257;
  huff->nodeList[495].parent = &huff->nodeList[504];
  huff->nodeList[495].weight = 154526;
  huff->nodeList[495].symbol = 257;
  huff->nodeList[496].parent = &huff->nodeList[504];
  huff->nodeList[496].weight = 162099;
  huff->nodeList[496].symbol = 257;
  huff->nodeList[497].parent = &huff->nodeList[505];
  huff->nodeList[497].weight = 176149;
  huff->nodeList[497].symbol = 257;
  huff->nodeList[498].parent = &huff->nodeList[505];
  huff->nodeList[498].weight = 191097;
  huff->nodeList[498].symbol = 257;
  huff->nodeList[510].left = &huff->nodeList[506];
  huff->nodeList[499].parent = &huff->nodeList[506];
  huff->nodeList[499].weight = 200130;
  huff->nodeList[499].symbol = 257;
  huff->nodeList[500].parent = &huff->nodeList[506];
  huff->nodeList[500].weight = 217930;
  huff->nodeList[500].symbol = 257;
  huff->nodeList[510].right = &huff->nodeList[507];
  huff->nodeList[501].parent = &huff->nodeList[507];
  huff->nodeList[501].weight = 234001;
  huff->nodeList[501].symbol = 257;
  huff->nodeList[502].parent = &huff->nodeList[507];
  huff->nodeList[502].weight = 258448;
  huff->nodeList[502].symbol = 257;
  huff->nodeList[503].parent = huff->nodeList[511].left;
  huff->nodeList[503].weight = 285792;
  huff->nodeList[503].symbol = 257;
  huff->nodeList[511].right = &huff->nodeList[509];
  huff->nodeList[504].parent = &huff->nodeList[509];
  huff->nodeList[504].weight = 316625;
  huff->nodeList[504].symbol = 257;
  huff->nodeList[505].parent = &huff->nodeList[509];
  huff->nodeList[505].weight = 367246;
  huff->nodeList[505].symbol = 257;
  huff->nodeList[512].left = &huff->nodeList[510];
  huff->nodeList[506].parent = &huff->nodeList[510];
  huff->nodeList[506].weight = 418060;
  huff->nodeList[506].symbol = 257;
  huff->nodeList[507].left = &huff->nodeList[501];
  huff->nodeList[507].right = &huff->nodeList[502];
  huff->nodeList[507].parent = &huff->nodeList[510];
  huff->nodeList[507].weight = 492449;
  huff->nodeList[507].symbol = 257;
  huff->nodeList[508].parent = &huff->nodeList[511];
  huff->nodeList[508].weight = 559846;
  huff->nodeList[508].symbol = 257;
  huff->nodeList[509].left = &huff->nodeList[504];
  huff->nodeList[509].right = &huff->nodeList[505];
  huff->nodeList[509].parent = &huff->nodeList[511];
  huff->nodeList[509].weight = 683871;
  huff->nodeList[509].symbol = 257;
  huff->nodeList[510].parent = huff->nodeList[511].parent;
  huff->nodeList[510].weight = 910509;
  huff->nodeList[510].symbol = 257;
  huff->nodeList[511].weight = 1243717;
  huff->nodeList[511].symbol = 257;
  huff->nodeList[512].right = &huff->nodeList[511];
  huff->nodeList[512].weight = 2154226;
  huff->nodeList[512].symbol = 257;
}


void MSG_InitHuffman() {

	static qboolean huffInit = qfalse;

	if (huffInit)
		return;

	huffInit = qtrue;
	Huff_Init(&msgHuff);
	Huff_BuildFromData(&msgHuff, msg_hData);
}
