#include <audio/sidfx.h>


const SIDFX	SFXClick[1] = {{
	2000, 2048, 
	SID_CTRL_RECT | SID_CTRL_GATE,
	SID_ATK_2 | SID_DKY_48,
	SID_DKY_300 | 0xd0,
	100, 0,
	4, 8,
	20
}};

const SIDFX	SIDFXBounce[1] = {{
	1000, 2048, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_750,
	-20, 0,
	4, 20,
	3
}};

// Sound effect exploding mine
const SIDFX	SIDFXMineExplosion[1] = {{
	3000, 1000, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_3000,
	-10, 0,
	8, 120
}};

// Sound effect short count down beep
const SIDFX	SIDFXBeepShort[1] = {{
	8000, 2048, 
	SID_CTRL_GATE | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_72,
	0, 0,
	16, 4,
	1
}};

// Sound effect hurry up beep
const SIDFX	SIDFXBeepHurry[1] = {{
	8000, 2048, 
	SID_CTRL_GATE | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_72,
	0, 0,
	8, 4,
	2
}};

// Sound effect long start beep
const SIDFX	SIDFXBeepLong[1] = {{
	12000, 2048, 
	SID_CTRL_GATE | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_750,
	0, 0,
	20, 32,
	1
}};

SIDFX	SIDFXMower[2] = {{
	3000, 4096,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_72,
	0xf0  | SID_DKY_300,
	-400, 0,
	2, 0,
	10
},{
	5000, 4096,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_72,
	0xf0  | SID_DKY_300,
	-400, 0,
	2, 2,
	10
}};
// Sound effect for enemy explosion
SIDFX	SIDFXEnemyExplosion[1] = {{
	1000, 1000, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_1500,
	-20, 0,
	8, 40,
	5
}};

// Sound effect for player explosion
SIDFX	SIDFXPlayerExplosion[4] = {{
	100, 1000, 
	SID_CTRL_GATE | SID_CTRL_SAW,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_1500,
	0, 0,
	2, 0,
	10
},{
	300, 1000, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_1500,
	100, 0,
	8, 0,
	10
},{
	1000, 1000, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_1500,
	200, 0,
	4, 20,
	10
},{
	2000, 1000, 
	SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_1500,
	0, 0,
	0, 40,
	10
}};

// Sound effect for bouncing agains wall or floor
SIDFX	SIDFXBoing[1] = {{
	3200, 2048 + 300, 
	SID_CTRL_GATE | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_750,
	-80, -40,
	4, 60,
	1
}};

// Sound effect for capturing a coin
SIDFX	SIDFXKatching[5] = {{
	NOTE_C(10), 512, 
	SID_CTRL_GATE | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_114,
	0x80  | SID_DKY_114,
	0, 0,
	1, 0,
	2
},{
	NOTE_C(9), 512, 
	SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_114,
	0x80  | SID_DKY_114,
	0, 0,
	1, 0,
	2
},{
	NOTE_G(10), 512, 
	SID_CTRL_GATE | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_114,
	0x80  | SID_DKY_114,
	0, 0,
	1, 0,
	2
},{
	NOTE_G(9), 512, 
	SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_114,
	0x80  | SID_DKY_114,
	0, 0,
	1, 0,
	2
},{
	NOTE_E(10), 512, 
	SID_CTRL_GATE | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_750,
	0xf0  | SID_DKY_750,
	0, 16,
	4, 60,
	2
}};

// Sound effect for catching a star
SIDFX	SIDFXStar[4] = {{
	NOTE_C(8), 3072, 
	SID_CTRL_GATE | SID_CTRL_SAW,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_6,
	0, 0,
	1, 0,
	4
},{
	NOTE_E(8), 3072, 
	SID_CTRL_GATE | SID_CTRL_SAW,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_6,
	0, 0,
	1, 0,
	4
},{
	NOTE_G(8), 3072, 
	SID_CTRL_GATE | SID_CTRL_SAW,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_6,
	0, 0,
	1, 0,
	4
},{
	NOTE_C(9), 3072, 
	SID_CTRL_GATE | SID_CTRL_SAW,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_300,
	0, 0,
	4, 32,
	4
}};

// Sound effect of flying shuriken
SIDFX	SIDFXShuriken[4] = {{
	8000, 3072, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_168,
	-400, 0,
	6, 0,
	7
},{
	8000, 3072, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xc0  | SID_DKY_168,
	-500, 0,
	6, 0,
	7
},{
	8000, 3072, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xa0  | SID_DKY_168,
	-600, 0,
	5, 0,
	3
},{
	8000, 3072, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0x80  | SID_DKY_750,
	-800, 0,
	1, 8,
	3
}};
SIDFX	SIDFXOtherExplosion[2] = {{
	2000, 0,
	SID_CTRL_GATE | SID_CTRL_SAW,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_750,
	-50, 0,
	2, 0,
	100
},{
	2000, 0,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_2400,
	-10, 0,
	5, 200,
	50
}};

SIDFX	SIDFXShot[1] = {{
	8000, 0,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_750,
	-100, 0,
	2, 20,
	10
}};

SIDFX	SIDFXFlagCapture[4] = {{
	NOTE_C(8), 3072, 
	SID_CTRL_GATE | SID_CTRL_SAW,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_6,
	0, 0,
	1, 0,
	4
},{
	NOTE_E(8), 3072, 
	SID_CTRL_GATE | SID_CTRL_SAW,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_6,
	0, 0,
	1, 0,
	4
},{
	NOTE_G(8), 3072, 
	SID_CTRL_GATE | SID_CTRL_SAW,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_6,
	0, 0,
	1, 0,
	4
},{
	NOTE_C(9), 3072, 
	SID_CTRL_GATE | SID_CTRL_SAW,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_300,
	0, 0,
	4, 32,
	4
}};

SIDFX	SIDFXFlagArrival[7] = {{
	NOTE_E(9), 1840, 
	SID_CTRL_GATE | SID_CTRL_TRI | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_300,
	0x00  | SID_DKY_300,
	0, 0,
	1, 4,
	4
},{
	NOTE_G(9), 1840, 
	SID_CTRL_GATE | SID_CTRL_TRI | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_300,
	0x00  | SID_DKY_300,
	0, 0,
	1, 1,
	4
},{
	NOTE_A(9), 1840, 
	SID_CTRL_GATE | SID_CTRL_TRI | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_300,
	0x00  | SID_DKY_300,
	0, 0,
	1, 1,
	4
},{
	NOTE_B(9), 1840, 
	SID_CTRL_GATE | SID_CTRL_TRI | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_300,
	0x00  | SID_DKY_300,
	0, 0,
	1, 4,
	4
},{
	NOTE_E(10), 1840, 
	SID_CTRL_GATE | SID_CTRL_TRI | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_300,
	0x00  | SID_DKY_300,
	0, 0,
	1, 4,
	4
},{
	NOTE_D(10), 1840, 
	SID_CTRL_GATE | SID_CTRL_TRI | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_300,
	0x00  | SID_DKY_300,
	0, 0,
	1, 4,
	4
},{
	NOTE_B(9), 1840, 
	SID_CTRL_GATE | SID_CTRL_TRI | SID_CTRL_RECT,
	SID_ATK_2 | SID_DKY_300,
	0x00  | SID_DKY_300,
	0, 0,
	1, 32,
	4
}};



void sfx_flag_arrival(void)
{
	sidfx_play(2, SIDFXFlagArrival, 7);
}

SIDFX	SIDFXZombieFume[1] = {{
	6000, 4096,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_240,
	-400, 0,
	2, 12,
	10
}};

SIDFX	SIDFXZombieChomp[1] = {{
	2000, 4096,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_240,
	-100, 0,
	2, 12,
	0
}};

SIDFX	SIDFXFire[1] = {{
	8000, 1000, 
	SID_CTRL_GATE | SID_CTRL_SAW,
	SID_ATK_16 | SID_DKY_114,
	0x40  | SID_DKY_750,
	-80, 0,
	4, 30
}};



// Sound effect for player explosion
SIDFX	SIDFXBigExplosion[3] = {
	{
	1000, 1000, 
	SID_CTRL_GATE | SID_CTRL_SAW,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_168,
	-20, 0,
	4, 0
	},
	{
	1000, 1000, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_168,
	-20, 0,
	10, 0
	},
	{
	1000, 1000, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_1500,
	-10, 0,
	8, 80
	},	
};

SIDFX	SFXMessage[3] = {
    {
        NOTE_C(8), 2048, 
        SID_CTRL_RECT | SID_CTRL_GATE,
        SID_ATK_2 | SID_DKY_48,
        SID_DKY_300 | 0xd0,
        0, 0,
        2, 0,
        15
    },
    {
	    NOTE_E(8), 2048, 
        SID_CTRL_RECT | SID_CTRL_GATE,
        SID_ATK_2 | SID_DKY_48,
        SID_DKY_300 | 0xd0,
        0, 0,
        2, 0,
        15
    },
    {
        NOTE_C(9), 2048, 
        SID_CTRL_RECT | SID_CTRL_GATE,
        SID_ATK_2 | SID_DKY_48,
        SID_DKY_750 | 0xd0,
        0, 0,
        2, 16,
        5
    }
};

SIDFX	SIDFXZombieHit[1] = {{
	4000, 4096,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_168,
	-400, 0,
	2, 6,
	10
}};

SIDFX	SIDFXPlanted[1] = {{
	20000, 4096,
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_16 | SID_DKY_6,
	0xf0  | SID_DKY_240,
	-400, 0,
	2, 16,
	20
}};

SIDFX	SIDFXExplosion[1] = {{
	1000, 1000, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_1500,
	-20, 0,
	8, 40,
	30
}};

SIDFX	SIDFXSunshine[4] = {{
	NOTE_C(8), 4096,
	SID_CTRL_GATE | SID_CTRL_TRI,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_6,
	0, 0,
	1, 0,
	5
},{
	NOTE_E(8), 4096,
	SID_CTRL_GATE | SID_CTRL_TRI,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_6,
	0, 0,
	1, 0,
	5
},{
	NOTE_G(8), 4096,
	SID_CTRL_GATE | SID_CTRL_TRI,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_6,
	0, 0,
	1, 0,
	5
},{
	NOTE_C(9), 4096,
	SID_CTRL_GATE | SID_CTRL_TRI,
	SID_ATK_16 | SID_DKY_6,
	0xf0  | SID_DKY_168,
	0, 0,
	4, 16,
	5
}};

SIDFX	SFXTap[1] = {{
	400, 2048, 
	SID_CTRL_RECT | SID_CTRL_GATE,
	SID_ATK_2 | SID_DKY_48,
	SID_DKY_114 | 0xf0,
	0, 0,
	2, 2,
	0
}};



SIDFX SFXOpening[13] = {
    {
        NOTE_E(8), 2048,
        SID_CTRL_GATE | SID_CTRL_RECT,
        //SID_ATK_16 | SID_DKY_6,
        0x0f,
        //0xf0  | SID_DKY_168,
        0x00,
        0, 0,
        4, 8,
        5
    },
    {
        NOTE_D(8), 2048,
        SID_CTRL_GATE | SID_CTRL_RECT,
        SID_ATK_16 | SID_DKY_6,
        0xf0  | SID_DKY_168,
        0, 0,
        4, 8,
        5
    },
        {
        NOTE_C(8), 2048,
        SID_CTRL_GATE | SID_CTRL_RECT,
        SID_ATK_16 | SID_DKY_6,
        0xf0  | SID_DKY_168,
        0, 0,
        4, 8,
        5
    },
        {
        NOTE_B(7), 2048,
        SID_CTRL_GATE | SID_CTRL_RECT,
        SID_ATK_16 | SID_DKY_6,
        0xf0  | SID_DKY_168,
        0, 0,
        4, 8,
        5
    },
{
        NOTE_E(8), 2048,
        SID_CTRL_GATE | SID_CTRL_RECT,
        //SID_ATK_16 | SID_DKY_6,
        0x0f,
        //0xf0  | SID_DKY_168,
        0x00,
        0, 0,
        4, 8,
        5
    },
    {
        NOTE_D(8), 2048,
        SID_CTRL_GATE | SID_CTRL_RECT,
        SID_ATK_16 | SID_DKY_6,
        0xf0  | SID_DKY_168,
        0, 0,
        4, 8,
        5
    },
        {
        NOTE_C(8), 2048,
        SID_CTRL_GATE | SID_CTRL_RECT,
        SID_ATK_16 | SID_DKY_6,
        0xf0  | SID_DKY_168,
        0, 0,
        4, 8,
        5
    },
        {
        NOTE_B(7), 2048,
        SID_CTRL_GATE | SID_CTRL_RECT,
        SID_ATK_16 | SID_DKY_6,
        0xf0  | SID_DKY_168,
        0, 0,
        4, 8,
        5
    },        
{
        NOTE_E(8), 2048,
        SID_CTRL_GATE | SID_CTRL_RECT,
        //SID_ATK_16 | SID_DKY_6,
        0x0f,
        //0xf0  | SID_DKY_168,
        0x00,
        0, 0,
        4, 8,
        5
    },
    {
        NOTE_D(8), 2048,
        SID_CTRL_GATE | SID_CTRL_RECT,
        SID_ATK_16 | SID_DKY_6,
        0xf0  | SID_DKY_168,
        0, 0,
        4, 8,
        5
    },
        {
        NOTE_C(8), 2048,
        SID_CTRL_GATE | SID_CTRL_RECT,
        SID_ATK_16 | SID_DKY_6,
        0xf0  | SID_DKY_168,
        0, 0,
        4, 8,
        5
    },
        {
        NOTE_B(7), 2048,
        SID_CTRL_GATE | SID_CTRL_RECT,
        SID_ATK_16 | SID_DKY_6,
        0xf0  | SID_DKY_168,
        0, 0,
        4, 8,
        5
    },
    {
        NOTE_A(7), 2048,
        SID_CTRL_GATE | SID_CTRL_RECT,
        SID_ATK_16 | SID_DKY_6,
        0xf0 | SID_DKY_168,
        0, 0,
        4, 8,
        5
    }
};

SIDFX	SIDFXQuickExplosion[1] = {{
	1000, 1000, 
	SID_CTRL_GATE | SID_CTRL_NOISE,
	SID_ATK_2 | SID_DKY_6,
	0xf0  | SID_DKY_1500,
	-20, 0,
	4, 8,
	30
}};